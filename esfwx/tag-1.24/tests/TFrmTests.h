//---------------------------------------------------------------------------

#ifndef TFrmTestsH
#define TFrmTestsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Memo.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMX.Types.hpp>
//---------------------------------------------------------------------------
class TFrmTests : public TForm
{
__published:	// IDE-managed Components
  TMemo *memo_;
  void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFrmTests(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmTests *FrmTests;
//---------------------------------------------------------------------------
#endif
