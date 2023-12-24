#pragma once

namespace tool::gsc::opcode {

	enum VM : BYTE {
		VM_T8 = 0x36,
		VM_T2020 = 0x37,
		VM_T9 = 0x38
	};

	enum Platform : BYTE {
		PLATFORM_UNKNOWN = 0,
		PLATFORM_PC,
		PLATFORM_XBOX,
		PLATFORM_PLAYSTATION,
	};

	Platform PlatformOf(LPCCH name);
	LPCCH PlatformName(Platform plt);

	enum OPCode : UINT16 {
		OPCODE_Undefined,
		OPCODE_Unknown0,
		OPCODE_Unknown1,
		OPCODE_Unknown2,
		OPCODE_Unknown3,
		OPCODE_Unknown4,
		OPCODE_Unknown5,
		OPCODE_Unknown6,
		OPCODE_Unknown7,
		OPCODE_Unknowna,
		OPCODE_Unknownb,
		OPCODE_Unknown38,
		OPCODE_Nop,
		OPCODE_GetUndefined,
		OPCODE_Unknown10e,
		OPCODE_Unknown126,
		OPCODE_End,
		OPCODE_EvalLocalVariableCachedDebug,
		OPCODE_EvalLocalVariableRefCachedDebug,
		OPCODE_ClearParams,
		OPCODE_CheckClearParams,
		OPCODE_PreScriptCall,
		OPCODE_CallBuiltinFunction,
		OPCODE_CallBuiltinMethod,
		OPCODE_EmptyArray,
		OPCODE_WaitTill,
		OPCODE_ClearFieldVariableOnStack,
		OPCODE_ClearArray,
		OPCODE_ScriptMethodThreadCallPointer,
		OPCODE_WaitTillMatchTimeout,
		OPCODE_GetHash,
		OPCODE_WaitFrame,
		OPCODE_ScriptMethodThreadCallPointerEndOn,
		OPCODE_WaitTillMatch,
		OPCODE_ScriptThreadCallEndOn,
		OPCODE_Switch,
		OPCODE_ScriptFunctionCallPointer,
		OPCODE_DevblockBegin,
		OPCODE_NotEqual,
		OPCODE_ScriptMethodCallPointer,
		OPCODE_JumpOnTrue,
		OPCODE_ScriptMethodThreadCallEndOn,
		OPCODE_SafeCreateLocalVariables,
		OPCODE_GetNegUnsignedShort,
		OPCODE_ClassFunctionCall,
		OPCODE_Return,
		OPCODE_EvalFieldVariableRef,
		OPCODE_SafeDecTop,
		OPCODE_Bit_Or,
		OPCODE_ScriptThreadCall,
		OPCODE_Bit_Xor,
		OPCODE_GetSelfObject,
		OPCODE_GetNegByte,
		OPCODE_ScriptThreadCallPointerEndOn,
		OPCODE_BoolComplement,
		OPCODE_IsDefined,
		OPCODE_AddToArray,
		OPCODE_Wait,
		OPCODE_SuperEqual,
		OPCODE_ScriptFunctionCall,
		OPCODE_JumpOnTrueExpr,
		OPCODE_CreateArray,
		OPCODE_Inc,
		OPCODE_ShiftLeft,
		OPCODE_JumpOnGreaterThan,
		OPCODE_Plus,
		OPCODE_CastAndEvalFieldVariable,
		OPCODE_ShiftRight,
		OPCODE_CreateStruct,
		OPCODE_CastCanon,
		OPCODE_GreaterThanOrEqualTo,
		OPCODE_GetUIntPtr,
		OPCODE_GetLongInteger,
		OPCODE_EvalArray,
		OPCODE_WaitTillFrameEnd,
		OPCODE_EndOnCallback,
		OPCODE_EndOn,
		OPCODE_SuperNotEqual,
		OPCODE_GetFloat,
		OPCODE_ProfileStart,
		OPCODE_GetString,
		OPCODE_BoolNot,
		OPCODE_CastBool,
		OPCODE_Equal,
		OPCODE_GetUnsignedInteger,
		OPCODE_WaittillTimeout,
		OPCODE_GreaterThan,
		OPCODE_Jump,
		OPCODE_Divide,
		OPCODE_EndSwitch,
		OPCODE_JumpOnFalse,
		OPCODE_JumpOnFalseExpr,
		OPCODE_Minus,
		OPCODE_ProfileStop,
		OPCODE_GetInteger,
		OPCODE_ClassFunctionThreadCall,
		OPCODE_VectorScale,
		OPCODE_Modulus,
		OPCODE_VectorConstant,
		OPCODE_SizeOf,
		OPCODE_Notify,
		OPCODE_Vector,
		OPCODE_Dec,
		OPCODE_JumpOnLessThan,
		OPCODE_Bit_And,
		OPCODE_GetObjectType,
		OPCODE_Multiply,
		OPCODE_EvalFieldVariableOnStack,
		OPCODE_GetTime,
		OPCODE_AddToStruct,
		OPCODE_ClassFunctionThreadCallEndOn,
		OPCODE_LessThanOrEqualTo,
		OPCODE_GetGlobal,
		OPCODE_GetZero,
		OPCODE_ClearFieldVariable,
		OPCODE_EvalFieldObjectFromRef,
		OPCODE_GetSelf,
		OPCODE_GetResolveFunction,
		OPCODE_GetGlobalObject,
		OPCODE_GetByte,
		OPCODE_CastFieldObject,
		OPCODE_ScriptThreadCallPointer,
		OPCODE_LessThan,
		OPCODE_ScriptMethodCall,
		OPCODE_DecTop,
		OPCODE_GetVector,
		OPCODE_ScriptMethodThreadCall,
		OPCODE_JumpPush,
		OPCODE_GetUnsignedShort,
		OPCODE_EvalLocalVariableCached,
		OPCODE_EvalFieldVariable,
		OPCODE_GetFunction,
		OPCODE_EvalArrayRef,
		OPCODE_SetNextArrayKeyCached,
		OPCODE_Unknown9e,
		OPCODE_EvalLocalVariableCachedSafe,
		OPCODE_SetLocalVariableCachedOnStack,
		OPCODE_Unknownc7,
		OPCODE_Unknown35,
		OPCODE_FirstArrayKey,
		OPCODE_EvalFieldVariableOnStackRef,
		OPCODE_SetVariableFieldRef,
		OPCODE_SetVariableField,
		OPCODE_EvalSelfFieldVariable,
		OPCODE_SetLocalVariableCached,
		OPCODE_FirstArrayKeyCached,
		OPCODE_EvalLocalVariableRefCached,
		OPCODE_EvalGlobalObjectFieldVariable,
		OPCODE_EvalLocalVariableDefined,

		// T8-Compiler Custom opcodes
		OPCODE_T8C_GetLazyFunction,
	};
	void RegisterOpCodesMap();

	LPCCH OpCodeName(OPCode op);
}