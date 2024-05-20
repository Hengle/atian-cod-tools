#pragma once

namespace tool::gsc::opcode {

	enum VM : byte {
		VM_UNKNOWN = 0,
		VM_T7 = 0x1C,
		VM_T8 = 0x36,
		VM_T937 = 0x37,
		VM_T9 = 0x38,

		VM_MW23 = 0xF0,
	};

	enum Platform : byte {
		PLATFORM_UNKNOWN = 0,
		PLATFORM_PC = 1,
		PLATFORM_PLAYSTATION = 2,
		PLATFORM_XBOX = 3,
		PLATFORM_COUNT,
	};

	Platform PlatformOf(const char* name);
	const char* PlatformName(Platform plt);
	VM VMOf(const char* name);

	enum OPCode : uint16_t {
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
		OPCODE_EmptyStruct,
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

		// T9
		OPCODE_ProfileNamedStart,
		OPCODE_ProfileNamedEnd,
		OPCODE_GetNegUnsignedInteger,
		OPCODE_T9_EvalFieldVariableFromObjectCached,
		OPCODE_T9_EvalFieldVariableFromObjectFromRef,
		OPCODE_T9_SetFieldVariableFromObjectFromRef,
		OPCODE_T9_EvalLocalVariableCachedDouble,
		OPCODE_T9_EvalFieldVariableFromGlobalObject,
		OPCODE_T9_SetVariableFieldFromEvalArrayRef,
		OPCODE_T9_IncLocalVariableCached,
		OPCODE_T9_DecLocalVariableCached,
		OPCODE_T9_EvalArrayCached,
		OPCODE_T9_EndOnCallbackParam,
		OPCODE_T9_GetVarRef,
		OPCODE_T9_IteratorKey,
		OPCODE_T9_IteratorVal,
		OPCODE_T9_IteratorNext,

		// MW
		OPCODE_IW_RegisterVariable,
		OPCODE_IW_GetAnimation,
		OPCODE_IW_GetAnimationTree,
		OPCODE_IW_GetBuiltinFunction,
		OPCODE_IW_GetBuiltinMethod,
		OPCODE_IW_GetIString,
		OPCODE_IW_GetEmptyUnkE,
		OPCODE_IW_GetUnkb,
		OPCODE_IW_GetUnk9,
		OPCODE_IW_GetDVarHash,
		OPCODE_IW_EvalLocalVariableCached0,
		OPCODE_IW_EvalLocalVariableCached1,
		OPCODE_IW_EvalLocalVariableCached2,
		OPCODE_IW_EvalLocalVariableCached3,
		OPCODE_IW_EvalLocalVariableCached4,
		OPCODE_IW_EvalLocalVariableCached5,
		OPCODE_IW_EvalLocalArrayCached,
		OPCODE_IW_EvalLocalVariableObjectCached,
		OPCODE_IW_ClearFieldVariableRef,
		OPCODE_IW_EvalLevelFieldVariableRef,
		OPCODE_IW_EvalLevelFieldVariable,
		OPCODE_IW_EvalSelfFieldVar,
		OPCODE_IW_EvalSelfFieldVarRef,
		OPCODE_IW_EvalAnimFieldVar,
		OPCODE_IW_EvalAnimFieldVarRef,
		OPCODE_IW_SetAnimFieldVar,
		OPCODE_IW_SetLevelFieldVariable,
		OPCODE_IW_SetSelfFieldVar,
		OPCODE_IW_DevBlock,
		OPCODE_IW_SingleEndon,
		OPCODE_IW_SingleWaitTill,
		OPCODE_IW_IsTrue,
		OPCODE_IW_Switch,
		OPCODE_IW_EndSwitch,
		OPCODE_IW_SetWaittillVariableFieldCached,
		OPCODE_IW_WaitFrame,
		OPCODE_IW_AddToStruct,
		OPCODE_IW_BuiltinFunctionCallPointer,
		OPCODE_IW_BuiltinMethodCallPointer,

		OPCODE_IW_GetLevel,
		OPCODE_IW_GetLevelGRef,
		OPCODE_IW_GetGame,
		OPCODE_IW_GetGameRef,
		OPCODE_IW_GetAnim,
		OPCODE_IW_GetAnimRef,
		OPCODE_IW_EvalArrayCachedField,
		OPCODE_IW_GetThread,
		OPCODE_IW_Notify,
		OPCODE_IW_WaitTillMatch,

		OPCODE_DevOp,


		OPCODE_COUNT,
	};
	void RegisterOpCodesMap();

	const char* OpCodeName(OPCode op);
}