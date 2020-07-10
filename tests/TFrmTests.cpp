#include "test_android_pch.h"
#pragma hdrstop

#include "TFrmTests.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TFrmTests *FrmTests;
//---------------------------------------------------------------------------

__fastcall TFrmTests::TFrmTests(TComponent* Owner) :
TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFrmTests::FormActivate(TObject *Sender)
{
  EsUartEnumerator enu;
  enu.enumerate();
  const EsUartInfos& result = enu.resultGet();

  for(size_t idx = 0; idx < result.size(); ++idx)
  {
    EsUartInfo::Ptr info = result[idx];

    memo_->Lines->Add(
      EsString::format(
        esT("//-------------------------\n")
        esT("Name: '%s'\n")
        esT("Friendly: '%s'\n")
        esT("Path: '%s'\n")
        esT("Instance: '%s'\n")
        esT("IsModem? %s; IsUSB? %s\n"),
        info->nameGet().c_str(),
        info->friendlyNameGet().c_str(),
        info->devicePathGet().c_str(),
        info->instancePathGet().c_str(),
        info->isModem() ? esT("yes") : esT("no"),
        info->isUSBdevice() ? esT("yes") : esT("no")
      ).c_str()
    );
  }
}
//---------------------------------------------------------------------------
