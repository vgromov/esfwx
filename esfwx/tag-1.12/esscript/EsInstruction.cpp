#include "stdafx.h"
#pragma hdrstop

EsInstruction::EsInstruction() :
m_opcode(iInvalid)
{
}

EsInstruction::EsInstruction(EsInstructionOpcode opcode, const EsVariant& param) :
m_opcode(opcode),
m_param(param)
{
}

const EsString::const_pointer c_opStrings[iOpcodesCount] = {
	wxT("iInvalid"),
	wxT("iVarInit"),
	wxT("iLoadConstVal"),							
	wxT("iLoadVarRef"),
	wxT("iFuncCall"),	
	wxT("iExtFuncCall"),
	wxT("iMethodCall"),
	wxT("iConstructorCall"),
	wxT("iRet"),	
	wxT("iRetCleanup"),
	wxT("iAsn"),
	wxT("iBinOpExpr"),
	wxT("iUnaryOpExpr"),
	wxT("iLogicCheck"),
	wxT("iIncDec"),
	wxT("iLoadItemRef"),
	wxT("iLoadFieldRef"),
	wxT("iLoadThisFieldRef"),
	wxT("iLoadPropRef"),
	wxT("iLoadThisPropRef"),
	wxT("iLoadAttributeVal"),
	wxT("iLoadThisAttributeVal"),
	wxT("iJump"),
	wxT("iJumpFalse"),
};

// get opcode string representation
EsString::const_pointer EsInstruction::getOpcodeString( EsInstructionOpcode opcode )
{
	wxASSERT(opcode != iOpcodesCount);
	return c_opStrings[opcode];
}