#include <includes.hpp>

int tool::dump::poolscripts(const process& proc, int argc, const char* argv[]) {
    uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[OFFSET_XASSET_SCRIPTPARSETREE]);
    INT32 poolSize = proc.ReadMemory<INT32>(proc[OFFSET_XASSET_SCRIPTPARSETREE + 0x14]);
    T8ScriptParseTreeEntry* buffer = new T8ScriptParseTreeEntry[poolSize];

    std::cout << std::hex << "pool: " << poolPtr << ", elements: " << std::dec << poolSize << "\n";

    if (!proc.ReadMemory(buffer, poolPtr, sizeof *buffer * poolSize)) {
        std::cerr << "Can't read pool data\n";
        delete[] buffer;
        return -1;
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    LPCCH outFile;
    if (argc == 2) {
        outFile = "pool.csv";
    }
    else {
        outFile = argv[2];
    }

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
        return -1;
    }

    // csv header
    out << "index,name,size,buffer\n";

    for (size_t i = 0; i < poolSize; i++) {
        const auto& ref = buffer[i];
        out << i << "," << hashutils::ExtractTmp("script", ref.name) << "," << ref.size << "," << std::hex << ref.buffer << std::dec << "\n";
    }


    out.close();
    delete[] buffer;

    return 0;
}

int tool::dump::writepoolscripts(const process& proc, int argc, const char* argv[]) {
    uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[OFFSET_XASSET_SCRIPTPARSETREE]);
    INT32 poolSize = proc.ReadMemory<INT32>(proc[OFFSET_XASSET_SCRIPTPARSETREE + 0x14]);
    T8ScriptParseTreeEntry* buffer = new T8ScriptParseTreeEntry[poolSize];

    std::cout << std::hex << "pool: " << poolPtr << ", elements: " << std::dec << poolSize << "\n";

    if (!proc.ReadMemory(buffer, poolPtr, sizeof * buffer * poolSize)) {
        std::cerr << "Can't read pool data\n";
        delete[] buffer;
        return -1;
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    LPCCH outFile;
    if (argc == 2) {
        outFile = "scriptparsetree";
    }
    else {
        outFile = argv[2];
    }

    size_t allocated = 0x1000;
    void* storage = std::malloc(allocated);
    char nameBuffer[1000];

    for (size_t i = 0; i < poolSize; i++) {
        const auto& ref = buffer[i];

        if (allocated < ref.size) {
            void* ns = std::realloc(storage, ref.size + 100);
            if (ns) {
                allocated = ref.size + 100;
                storage = ns;
            }
            else {
                std::cerr << "Can't allocate buffer for address " << std::hex << ref.buffer << " of size " << std::dec << ref.size << "\n";
                continue; // bad size?
            }
        }

        if (!proc.ReadMemory(storage, ref.buffer, ref.size)) {
            std::cerr << "Can't read pooled buffer at address " << std::hex << ref.buffer << " of size "<< std::dec << ref.size << "\n";
            continue;
        }

        LPCCH name = hashutils::ExtractPtr(ref.name);
        if (name) {
            snprintf(nameBuffer, 1000, "%s/%sbin", outFile, name);
        }
        else {
            // split into multiple directories to avoid creating a big directory
            snprintf(nameBuffer, 1000, "%s/hashed-%lld/script_%llx.gscbin", outFile, (ref.name % 3 + 1), ref.name);
        }

        std::filesystem::path file(nameBuffer);
        
        if (!std::filesystem::create_directories(file.parent_path())
            || !utils::WriteFile(file, storage, ref.size)) {
            std::cerr << "Error when writing " << nameBuffer << "\n";
        }
    }

    std::free(storage);
    delete[] buffer;

    return 0;
}

int tool::dump::linkedscripts(const process& proc, int argc, const char* argv[]) {
    UINT32 bufferCount[2];
    if (!proc.ReadMemory(bufferCount, proc[OFFSET_gObjFileInfoCount], sizeof *bufferCount * 2)) {
        std::cerr << "Can't read count data\n";
        return -1;
    }

    const int size = 650;
    T8ObjFileInfo* buffer = new T8ObjFileInfo[2 * size];

    if (!proc.ReadMemory(buffer, proc[OFFSET_gObjFileInfo], sizeof *buffer * size * 2)) {
        std::cerr << "Can't read linked data\n";
        delete[] buffer;
        return -1;
    }


    LPCCH outFile;
    if (argc == 2) {
        outFile = "linked.csv";
    }
    else {
        outFile = argv[2];
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
        return -1;
    }



    // csv header
    out << "pool,name,slot,refcount,groupid,address\n";

    T8GSCSimpleHeader gsc;
    const BYTE magic[8] = { 0x80, 0x47, 0x53, 0x43, 0x0D, 0x0A, 0x00, 0x36};
    const UINT64 magicLong = *(UINT64*)(&magic[0]);

    for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
        std::cout << std::dec << "Reading " << bufferCount[inst] << " " << scriptinstance::Name(inst) << " script(s)\n";
        for (size_t i = 0; i < bufferCount[inst]; i++) {
            const auto& ref = buffer[i + size * inst];

            if (!proc.ReadMemory(&gsc, ref.activeVersion, sizeof gsc)) {
                std::cerr << "Can't read memory from location " << std::hex << ref.activeVersion << ", index: " << i << "\n";
                continue;
            }
            if (gsc.magic != magicLong) {
                std::cerr << "Bad magic for location " << std::hex << ref.activeVersion << " " << gsc.magic << " != " << magicLong << "\n";
                continue;
            }

            out << std::dec << scriptinstance::Name(inst) 
                << "," << hashutils::ExtractTmp("script", gsc.name)
                << "," << ref.slot
                << "," << ref.refCount
                << "," << ref.groupId
                << "," << std::hex << ref.activeVersion
                << "\n";
        }
    }


    out.close();
    delete[] buffer;

    return 0;
}

int tool::dump::events(const process& proc, int argc, const char* argv[]) {
    const int size = 512;
    T8EventMapObj* buffer = new T8EventMapObj[2 * size];

    if (!proc.ReadMemory(buffer, proc[OFFSET_VM_EVENT], sizeof * buffer * size * 2)) {
        std::cerr << "Can't read event data\n";
        delete[] buffer;
        return -1;
    }

    LPCCH outFile;
    if (argc == 2) {
        outFile = "events.csv";
    }
    else {
        outFile = argv[2];
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
        return -1;
    }

    // csv header
    out << "pool,name,gamesideregisterd\n";

    int read = 0;

    for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
        for (size_t i = 0; i < size; i++) {
            const auto& ref = buffer[i + size * inst];
            if (!ref.name) {
                continue; // ignore empty
            }
            out << scriptinstance::Name(inst) << "," << hashutils::ExtractTmp("event", ref.name) << "," << (ref.gameSideRegistered ? "true" : "false") << "\n";
            read++;
        }
    }

    std::cout << std::dec << "Read " << read << " event(s)\n";


    out.close();
    delete[] buffer;

    return 0;
}

struct FunctionPoolDef {
    scriptinstance::scriptinstance instance;
    bool methodPool;
    UINT64 offset;
    UINT32 size;
};

/*
Can be extract from these functions, you can also search for the hash32("spawn") in the executable, 
it'll find the location of a pool search the x ref to it. (spawn is available in both CSC/GSC)
Scr_GetFunction sub_33AF840
Scr_GetMethod sub_33AFC20
CScr_GetFunction sub_1F13140
CScr_GetMethod sub_1F13650
*/
static FunctionPoolDef g_functionPool[] = {
    { scriptinstance::SI_SERVER, false, 0x49b60c0, 8 },
    { scriptinstance::SI_SERVER, false, 0x4f437c0, 370 },
    { scriptinstance::SI_SERVER, false, 0x49b9ae0, 372 },
    { scriptinstance::SI_SERVER, false, 0x49600d0, 60 },
    { scriptinstance::SI_SERVER, false, 0x495f250, 114 },
    { scriptinstance::SI_SERVER, false, 0x49608a0, 75 },
    { scriptinstance::SI_SERVER, false, 0x49612d0, 14 },
    { scriptinstance::SI_SERVER, false, 0x49b5590, 9 },
    { scriptinstance::SI_SERVER, false, 0x49b56c0, 15 },
    { scriptinstance::SI_SERVER, false, 0x495cfb0, 16 },
    { scriptinstance::SI_SERVER, false, 0x495ceb0, 6 },
    { scriptinstance::SI_SERVER, false, 0x495cf78, 1 },
    { scriptinstance::SI_SERVER, false, 0x495d1c8, 1 },
    { scriptinstance::SI_SERVER, true, 0x498dcf0, 491 },
    { scriptinstance::SI_SERVER, true, 0x49b2980, 40 },
    { scriptinstance::SI_SERVER, true, 0x49b5b20, 45 },
    { scriptinstance::SI_SERVER, true, 0x49bcff0, 19 },
    { scriptinstance::SI_SERVER, true, 0x49bd540, 134 },
    { scriptinstance::SI_SERVER, true, 0x49b3180, 5 },
    { scriptinstance::SI_SERVER, true, 0x49f1ff0, 123 },
    { scriptinstance::SI_SERVER, true, 0x495cd40, 10 },
    { scriptinstance::SI_SERVER, true, 0x49b3490, 260 },
    { scriptinstance::SI_SERVER, true, 0x49b58a0, 1 },
    { scriptinstance::SI_SERVER, true, 0x4f466b0, 372 },

    { scriptinstance::SI_CLIENT, false, 0x4ed3470, 351 },
    { scriptinstance::SI_CLIENT, false, 0x49600d0, 60 },
    { scriptinstance::SI_CLIENT, false, 0x495f250, 114 },
    { scriptinstance::SI_CLIENT, false, 0x49608a0, 75 },
    { scriptinstance::SI_CLIENT, false, 0x49612d0, 14 },
    { scriptinstance::SI_CLIENT, false, 0x4ed01a0, 135 },
    { scriptinstance::SI_CLIENT, false, 0x4969d70, 34 },
    { scriptinstance::SI_CLIENT, false, 0x4969bf0, 12 },
    { scriptinstance::SI_CLIENT, false, 0x496a370, 25 },
    { scriptinstance::SI_CLIENT, false, 0x496a720, 26 },
    { scriptinstance::SI_CLIENT, false, 0x4967da0, 31 },
    { scriptinstance::SI_CLIENT, true, 0x496e7d0, 325 },
    { scriptinstance::SI_CLIENT, true, 0x4969370, 56 },
    { scriptinstance::SI_CLIENT, true, 0x496a1b0, 9 },
    { scriptinstance::SI_CLIENT, true, 0x496aa60, 3 },
    { scriptinstance::SI_CLIENT, true, 0x4968180, 13 },
};


int tool::dump::dumpfunctions(const process& proc, int argc, const char* argv[]) {
    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    LPCCH outFile;
    if (argc == 2) {
        outFile = "funcs.csv";
    }
    else {
        outFile = argv[2];
    }

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        return -1;
    }

    // csv header
    out << "pool,func,minargs,maxargs,type,address\n";

    const size_t funcPoolSize = sizeof(g_functionPool) / sizeof(*g_functionPool);
    const size_t maxSize = 1000;
    T8BuiltInFunc* buffer = new T8BuiltInFunc[maxSize];
    std::cout << std::dec << "reading: " << funcPoolSize << " pools\n";

    for (size_t i = 0; i < funcPoolSize; i++) {
        const auto& pool = g_functionPool[i];
        std::cout << std::hex << "reading pool " << pool.offset << "\n";
        if (pool.size > maxSize) {
            std::cerr << "bad pool size\n";
            continue;
        }
        if (!proc.ReadMemory(buffer, proc[pool.offset], sizeof(*buffer) * pool.size)) {
            std::cerr << "Can't read pool data for pool" << std::hex << pool.offset << "\n";
            continue;
        }
        for (size_t j = 0; j < pool.size; j++) {
            const auto& func = buffer[j];
            out << std::hex << (pool.instance ? "CSC" : "GSC") << "-" << (pool.methodPool ? "Method" : "Function") << "-" << pool.offset
                << std::dec
                << "," << hashutils::ExtractTmp("function", func.name)
                << "," << func.minArgs
                << "," << func.maxArgs
                << "," << func.type
                << ",sub:" << std::hex << proc.GetModuleRelativeOffset(func.function)
                << "\n";
        }
    }

    out.close();
    delete[] buffer;

    return 0;
}
