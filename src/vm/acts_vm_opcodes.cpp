#include <includes_shared.hpp>
#include <utils.hpp>
#include "acts_vm.hpp"
#include "acts_vm_opcodes.hpp"
#include "acts_vm_registry.hpp"


namespace acts::vm::opcodes {
	constexpr size_t HANDLERS_MAX_OP = 0x100;
	typedef void(*OpcodeHandler)(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated);
	OpcodeHandler handlers[HANDLERS_MAX_OP];


	void InvalidOpCodeHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated) {
		*terminated = true;
		vm->Error("Invalid opcode", true);
	}

	void NopHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread*, bool*) {
		// nop
	}

	void ExportNoParams(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
		if (thread->top->type != VT_PRECALL) {
			do {
				vm->ReleaseVariable(thread->top--);
			} while (thread->top->type != VT_PRECALL);
			vm->Error("Called function with too many parameters", false);
		}
	}

	void ExportParams(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
		byte vars{ *(thread->codePos++) };

		VmVar* top{ thread->top };
		for (size_t i = 0; i < vars; i++) {
			uint32_t* baseName{ thread->SetAlignedData<uint32_t>() };
			uint32_t name = *baseName;
			thread->codePos = (byte*)(baseName + 1);


			// TODO: register var, check stack

		}

		if (top->type != VT_PRECALL) {
			do {
				vm->ReleaseVariable(thread->top--);
			} while (thread->top->type != VT_PRECALL);
			vm->Error("Called function with too many parameters", false);
		}
	}

	void GetIntHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
		int64_t* base{ thread->SetAlignedData<int64_t>() };
		int64_t i{ *base };
		thread->codePos = (byte*)(base + 1);
		VmVar* top{ thread->top++ };
		top->val.i = i;
		top->type = VT_INTEGER;
	}

	void GetFloatHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
		float* base{ thread->SetAlignedData<float>() };
		float f{ *base };
		thread->codePos = (byte*)(base + 1);
		VmVar* top{ thread->top++ };
		top->val.f = f;
		top->type = VT_FLOAT;
	}

	void GetHashHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
		uint64_t* base{ thread->SetAlignedData<uint64_t>() };
		uint64_t h{ *base };
		thread->codePos = (byte*)(base + 1);
		VmVar* top{ thread->top++ };
		top->val.hash = h;
		top->type = VT_HASH;
	}

	void GetUndefinedHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
		uint64_t* base{ thread->SetAlignedData<uint64_t>() };
		uint64_t h{ *base };
		thread->codePos = (byte*)(base + 1);
		VmVar* top{ thread->top++ };
		top->val.i = 0;
		top->type = VT_UNDEFINED;
	}

	void IsDefinedHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
		uint64_t* base{ thread->SetAlignedData<uint64_t>() };
		uint64_t h{ *base };
		thread->codePos = (byte*)(base + 1);
		VmVar* top{ thread->top };
		vm->ReleaseVariable(thread->top);
		top->val.b = thread->top->type != VT_UNDEFINED;
		top->type = VT_INTEGER;
	}

	static int handlersSet{
		([]() -> int {
			// Set default array
			for (size_t i = 0; i < HANDLERS_MAX_OP; i++) {
				handlers[i] = InvalidOpCodeHandler;
			}

			handlers[OpCodeId::OPCODE_NOP] = NopHandler;
			handlers[OpCodeId::OPCODE_EXPORT_NO_PARAMS] = ExportNoParams;
			handlers[OpCodeId::OPCODE_EXPORT_PARAMS] = ExportParams;
			//handlers[OpCodeId::OPCODE_END] = InvalidOpCodeHandler; // todo
			handlers[OpCodeId::OPCODE_GET_INT] = GetIntHandler;
			handlers[OpCodeId::OPCODE_GET_FLOAT] = GetFloatHandler;
			handlers[OpCodeId::OPCODE_GET_HASH] = GetHashHandler;
			handlers[OpCodeId::OPCODE_GET_UNDEFINED] = GetUndefinedHandler;

			handlers[OpCodeId::OPCODE_IS_DEFINED] = IsDefinedHandler;
			
			return 0;
		})()
	};

	void HandleOpCode(OpCode opcode, acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated) {
		if (opcode >= HANDLERS_MAX_OP) {
			InvalidOpCodeHandler(vm, thread, terminated);
			return;
		}
		handlers[opcode](vm, thread, terminated);
	}
}