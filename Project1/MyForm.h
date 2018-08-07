#pragma once
#include <comdef.h> 
#include "FreeImage.h"
#include "VTFLib.h"
namespace Project1 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->outFormat = FIF_UNKNOWN;
			FreeImage_Initialise();

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  comboBox1;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"TGA", L"PNG", L"VTF" });
			this->comboBox1->Location = System::Drawing::Point(62, 97);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 21);
			this->comboBox1->TabIndex = 1;
			this->comboBox1->SelectedValueChanged += gcnew System::EventHandler(this, &MyForm::comboBox1_SelectedValueChanged);
			// 
			// MyForm
			// 
			this->AllowDrop = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->ClientSize = System::Drawing::Size(247, 229);
			this->Controls->Add(this->comboBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"MyForm";
			this->Text = L"[GN]ImageCstrikeSource";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::MyForm_DragDrop);
			this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::MyForm_DragEnter);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void MyForm_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
		array<String^>^ paths = safe_cast<array<String^>^>(e->Data->GetData(DataFormats::FileDrop));
		for each (String^ path in paths) {
		
			Debug::WriteLine(path);
			this->doConvert(path);
		}
		 
	 
	}
	private: System::Void MyForm_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^ e) {
		if (e->Data->GetDataPresent(DataFormats::FileDrop)) e->Effect = DragDropEffects::Copy;
	}
	private: System::Void comboBox1_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e) {
		
		
	
	}
	
	void doConvert(String^ path)
	{
		if (this->comboBox1->Text->Equals("PNG"))
		{
			this->outFormat = FIF_PNG;
		}
		else if (this->comboBox1->Text->Equals("TGA")) {
			this->outFormat = FIF_TARGA;
		}
		else {
			this->outFormat = FIF_UNKNOWN;
		}
		String^ ext = System::IO::Path::GetExtension(path)->ToLower();
		
		if (ext->ToUpper()->Equals(".VTF"))
		{
			
			this->LoadVLib(path);
		}
		else  {
			this->LoadFreeImage(path);
		}

	}
	bool LoadFreeImage(String^ path)
	{
		String^ out = System::IO::Path::GetDirectoryName(path);
		String^ fileName = System::IO::Path::GetFileNameWithoutExtension(path);
		if (this->outFormat == FIF_PNG)
		{
			fileName += ".png";
		}
		else if (this->outFormat == FIF_TARGA)
			fileName += ".tga";
		else
			fileName += ".vtf";
		out += "\\" + fileName;


		

		IntPtr ptrPath2 = Marshal::StringToHGlobalAnsi(out);
		char * cPathOut = static_cast<char*>(ptrPath2.ToPointer());

		IntPtr ptrPath = Marshal::StringToHGlobalAnsi(path);
		char * cPath = static_cast<char*>(ptrPath.ToPointer());

		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(cPath);
	 

		FIBITMAP* bitmap = FreeImage_Load(format, cPath);

		int width = FreeImage_GetWidth(bitmap);
		int height = FreeImage_GetHeight(bitmap);
		int scan_width = FreeImage_GetPitch(bitmap);
		unsigned bpp = FreeImage_GetBPP(bitmap);
 
		// convert the bitmap to raw bits (top-left pixel first)
	
		 
		
		if (this->outFormat == FIF_UNKNOWN) {
			BYTE *bits = (BYTE*)malloc(height * scan_width);
		
			FreeImage_ConvertToRawBits(bits, bitmap, scan_width, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);

			for (int k = 0; k < height * scan_width; k+=4)
			{
				BYTE temp = bits[k];
				bits[k] = bits[k +2 ];
				bits[k + 2] = temp;
			}
		 
			vlUInt uiVTFImage;
			vlUInt uiVMTMaterial;
			SVTFCreateOptions CreateOptions;
			vlInitialize();

			vlCreateImage(&uiVTFImage);
			vlBindImage(uiVTFImage);

			vlCreateMaterial(&uiVMTMaterial);
			vlBindMaterial(uiVMTMaterial);

			CreateOptions.ImageFormat = IMAGE_FORMAT_DXT5;
			vlImageCreateDefaultCreateStructure(&CreateOptions);

			if (!vlImageCreateSingle(width, height, bits, &CreateOptions))
			{
				Debug::WriteLine("  Error creating vtf file: ", gcnew String(vlGetLastError()) );
				return false;
			}
			if (!vlImageSave(cPathOut))
			{
				Debug::WriteLine("  Error creating vtf file:", gcnew String(vlGetLastError()));
				return false;
			}
			vlDeleteMaterial(uiVMTMaterial);
			vlDeleteImage(uiVTFImage);
			vlShutdown();
		}
		else {

			FreeImage_Save(this->outFormat, bitmap, cPathOut);
		}
		FreeImage_Unload(bitmap);
		return true;

	}
	void FlipImage(vlByte *lpImageData, vlUInt uiWidth, vlUInt uiHeight, vlUInt uiChannels)
	{
		vlUInt i, j, k;
		vlByte bTemp;

		for (i = 0; i < uiWidth; i++)
		{
			for (j = 0; j < uiHeight / 2; j++)
			{
				vlByte *pOne = lpImageData + (i + j * uiWidth) * uiChannels;
				vlByte *pTwo = lpImageData + (i + (uiHeight - j - 1) * uiWidth) * uiChannels;

				for (k = 0; k < uiChannels; k++)
				{
					bTemp = pOne[k];
					pOne[k] = pTwo[k];
					pTwo[k] = bTemp;
				}
			}
		}
	}
	bool LoadVLib(String^ path)
	{
		if (this->outFormat == FIF_UNKNOWN) { 
			Debug::WriteLine("Loi ------- -1");
			return false; 
		}
		String^ out = System::IO::Path::GetDirectoryName(path);
		String^ fileName = System::IO::Path::GetFileNameWithoutExtension(path);
		if (this->outFormat == FIF_PNG)
		{
			fileName += ".png";
		}else fileName += ".tga";
		out += "\\"+fileName;
	
		vlUInt uiVTFImage;									 
		vlUInt uiVMTMaterial;							 
	 
		vlInitialize();

		vlCreateImage(&uiVTFImage);
		vlBindImage(uiVTFImage);

		vlCreateMaterial(&uiVMTMaterial);
		vlBindMaterial(uiVMTMaterial);

		VTFImageFormat DestFormat;	 
		vlSingle sR, sG, sB;		 
		vlByte *lpImageData;		 
		IntPtr ptrPath = Marshal::StringToHGlobalAnsi(path);
		char * cPath = static_cast<char*>(ptrPath.ToPointer());
		ptrPath = Marshal::StringToHGlobalAnsi(out);
		char * cPathOut = static_cast<char*>(ptrPath.ToPointer());

		Debug::WriteLine("\nSave To", gcnew String(cPathOut));

		if (!vlImageLoad(cPath, vlFalse))
		{
			Debug::WriteLine(gcnew String(vlGetLastError()));
			return false;
		}
		DestFormat = (vlImageGetFlags() & (TEXTUREFLAGS_ONEBITALPHA | TEXTUREFLAGS_EIGHTBITALPHA)) ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888;
		lpImageData = (vlByte *)malloc(vlImageComputeImageSize(vlImageGetWidth(), vlImageGetHeight(), 1, 1, DestFormat));
		if (lpImageData == 0)
		{
			Debug::WriteLine("Loi ------- 1");
			return false;
		}
		if (!vlImageConvert(vlImageGetData(0, 0, 0, 0), lpImageData, vlImageGetWidth(), vlImageGetHeight(), vlImageGetFormat(), DestFormat))
		{
			free(lpImageData);
			Debug::WriteLine("Loi ------- 2");
			return false;
		}
		FlipImage(lpImageData, vlImageGetWidth(), vlImageGetHeight(), DestFormat == IMAGE_FORMAT_RGBA8888 ? 4 : 3);
		freeImageSave(lpImageData, vlImageGetWidth(), vlImageGetHeight(), this->outFormat, cPathOut);
		vlDeleteMaterial(uiVMTMaterial);
		vlDeleteImage(uiVTFImage);
		vlShutdown();
		return true;
	}

	void freeImageSave(unsigned char *pBuffer, int pWidth,int pHeight, FREE_IMAGE_FORMAT format, char *out)
	{
		FIBITMAP *dst = FreeImage_ConvertFromRawBits(pBuffer, pWidth, pHeight, 4 * pWidth, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
 
		FreeImage_Save(format, dst, out);
 
		FreeImage_Unload(dst);
	}
	
	FREE_IMAGE_FORMAT outFormat;
		
private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
	this->comboBox1->SelectedIndex = 2;
}
};
}
