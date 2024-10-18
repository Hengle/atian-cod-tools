#include <includes_shared.hpp>
#include <utils.hpp>
#include "acts_vm.hpp"
#include "acts_vm_opcodes.hpp"

namespace acts::vm {
	ActsVm::ActsVm(ActsVmConfig cfg) : cfg(cfg) {
	}

	void ActsVm::AssertThreadStarted() {
		if (!currentThread) throw std::runtime_error("No thread started");
	}

	VmExecutionThread* ActsVm::AllocThread(byte* codePos) {
		for (VmExecutionThread& thread : threads) {
			if (thread.running) continue;

			thread.stack[0].type = VT_THREAD;
			thread.stack[0].val.ref = thread.threadId;
			thread.top = thread.stack + 1;
			thread.waitTime = 0;
			thread.waitFrameTime = 0;
			thread.running = true;
			thread.codePos = codePos;
			return currentThread;
		}
		Error("Can't alloc thread", true);
		return nullptr;
	}

	void ActsVm::Execute() {
		AssertThreadStarted();
		if (currentThread->waitFrameTime > 0) {
			if (--currentThread->waitFrameTime) {
				return; // wait using waitframe()
			}
		}
		else if (currentThread->waitFrameTime) {
			if (currentThread->waitFrameTime <= utils::GetTimestamp()) {
				return; // wait using wait()
			}
			else {
				currentThread->waitFrameTime = 0;
			}
		}

		bool terminated{};

		while (!terminated) {
			opcodes::OpCode* base{ currentThread->SetAlignedData<opcodes::OpCode>() };
			opcodes::OpCode op{ *base };
			currentThread->codePos = (byte*)(base + 1);

			opcodes::HandleOpCode(op, this, currentThread, &terminated);
		}
	}

	void ActsVm::ReleaseVariable(VmVar* var) {
		// todo: when pointer
	}
	bool ActsVm::GetFunctionInfo(byte* codePos, ScriptExport** exp, ActScript** script) {
		for (ScriptInfo& nfo : scripts) {
			if (!nfo.script->IsInScript(codePos)) {
				if (script) *script = nfo.script;
				uint32_t rloc = (uint32_t)(codePos - nfo.script->magic);

				if (exp) {
					*exp = nullptr;
					ScriptExport* e{ nfo.script->Exports() };
					ScriptExport* ee{ nfo.script->ExportsEnd() };

					while (e != ee) {
						if (
							e->address > rloc // too far
							&& (!*exp || (*exp)->address >= e->address) // before previous
							) {
							*exp = e;
						}
					}

					return *exp;
				}

				return true;
			}
		}

		return false;
	}
	void ActsVm::CleanupThread(VmExecutionThread* thread) {
		// todo
	}


	void ActsVm::Error(const char* msg, bool terminate) {
		if (currentThread) {
			ScriptExport* exp{};
			ActScript* script{};
			if (GetFunctionInfo(currentThread->codePos, &exp, &script)) {
				msg = utils::va("\n[%s<%s>::%s]", this->cfg.hashToString(exp->name_space), cfg.hashToString(script->name), cfg.hashToString(exp->name));
			}
		}
		if (terminate) {
			throw std::runtime_error(msg);
		}
		else {
			LOG_WARNING("{}", msg);
		}
	}

	VmVar* ActsVm::PushStack(size_t count) {
		AssertThreadStarted();
		if (!count) return currentThread->top;
		if (currentThread->top + count > &currentThread->stack[ARRAYSIZE(currentThread->stack)]) {
			Error("Invalid push: too much data", true);
		}
		VmVar* ptr = currentThread->top;
		currentThread->top += count;
		return ptr;
	}

	void ActsVm::PopStack(size_t count) {
		AssertThreadStarted();
		if (!count) return;
		if (currentThread->top - count < &currentThread->stack[0]) {
			Error("Invalid pop: not enough data", true);
		}

		currentThread->top -= count;
	}

	void ActsVm::AddInt(int64_t val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_INTEGER;
		ptr->val.i = val;
	}

	void ActsVm::AddFloat(float val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_FLOAT;
		ptr->val.f = val;
	}

	void ActsVm::AddHash(uint64_t val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_HASH;
		ptr->val.hash = val;
	}

	void ActsVm::LoadScript(uint64_t name) {
		linkGroup++;

		if (LinkScript(name) < 0) {
			throw std::runtime_error(utils::va("Can't load script %s", cfg.hashToString(name)));
		}

		while (topScripts < scripts.size()) {
			ScriptInfo& nfo{ scripts[topScripts++] };


			ScriptExport* exports = nfo.script->Exports();

			while (exports != nfo.script->ExportsEnd()) {
				if (exports->flags & ScriptExportFlags::SEF_AUTOEXEC) {
					currentThread = AllocThread(nfo.script->magic + exports->address);
					Execute();
				}
			}
		}
	}
	int ActsVm::LinkScript(uint64_t name) {
		ActScript* script = cfg.getterFunction(name);

		if (!script) {
			LOG_ERROR("Can't find script {}", cfg.hashToString(name));
			return ActsVmLinkOutput::VMLO_CANT_FIND;
		}

		if (*(uint64_t*)script != ACTSCRIPT_MAGIC) {
			LOG_ERROR("Invalid magic for script {}", cfg.hashToString(name));
			return ActsVmLinkOutput::VMLO_CANT_FIND;
		}

		auto itf = std::find_if(scripts.begin(), scripts.end(), [name](ScriptInfo& info) { return info.script->name == name; });

		if (itf != scripts.end()) {
			return ActsVmLinkOutput::VMLO_NOTHING; // already linked
		}

		ScriptInfo& nfo = scripts.emplace_back();
		nfo.script = script;
		nfo.group = linkGroup;

		// link script
		uint64_t* includes = (uint64_t*)(script->magic + script->includes_table);
		for (size_t i = 0; i < script->includes_count; i++) {
			if (LinkScript(includes[i]) < 0) {
				return ActsVmLinkOutput::VMLO_ERROR_INCLUDES;
			}
		}

		// TODO: link functions, strings, globals, etc.

		return 1 + script->includes_count;
	}
}