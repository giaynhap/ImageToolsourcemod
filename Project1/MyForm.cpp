#include "MyForm.h"
#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib, "VTFLib.lib")
using namespace System;
using namespace System::Windows::Forms;
[STAThread]
void main()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(true);
	Project1::MyForm form;
	Application::Run(%form);
}
