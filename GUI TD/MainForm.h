#pragma once

#include <msclr/marshal.h>
#include "Simulator.h"


namespace GUITD {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Description r�sum�e de MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	private: System::Windows::Forms::ListBox^ listMonitorsLabel;

	private: System::ComponentModel::IContainer^ components;
	private: System::Windows::Forms::MenuStrip^ menuStrip;

	private: System::Windows::Forms::ToolStripMenuItem^ fichierToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ quitterToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ howToUseItToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ aboutToolStripMenuItem;
	private: System::Windows::Forms::Button^ buttonAutoDetectMonitors;
	private: System::Windows::Forms::Panel^ panelHandleMonitor;

	private: System::Windows::Forms::Button^ buttonDetect;
	private: System::Windows::Forms::Panel^ panelError;
	private: System::Windows::Forms::Label^ labelError;




	private: System::Windows::Forms::Panel^ panelInformation;
	private: System::Windows::Forms::Label^ labelPosition;


	private: System::Windows::Forms::Label^ labelInformation;

	private: System::Windows::Forms::Panel^ panelPosition;
	private: System::Windows::Forms::TextBox^ textBoxPosY;
	private: System::Windows::Forms::TextBox^ textBoxPosX;
	private: System::Windows::Forms::Label^ labelInformationTitle;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::TextBox^ textBoxPrimaryMonitor;


	private: System::Windows::Forms::Label^ labelPrimaryMonitor;
	private: System::Windows::Forms::ToolStripMenuItem^ simulatorToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ launchToolStripMenuItem;
	private: System::Windows::Forms::Panel^ panel2;

	private: System::Windows::Forms::Label^ labelLayerInfo;
	private: System::Windows::Forms::ComboBox^ comboBoxLayer;
	private: System::Windows::Forms::ToolStripMenuItem^ propertiesToolStripMenuItem;
	private: System::Windows::Forms::Panel^ panelSimulator;
	private: System::Windows::Forms::TextBox^ textBoxFileProp;

	private: System::Windows::Forms::Label^ labelSimTitle;
	private: System::Windows::Forms::Button^ buttonSelectFileProp;
	private: System::Windows::Forms::Label^ labelFilePropTitle;
	private: System::Windows::Forms::OpenFileDialog^ openFileDialogFP;
	private: System::Windows::Forms::Button^ buttonLaunchSim;
	private: System::Windows::Forms::Button^ buttonStopSimulation;






	private: System::Windows::Forms::Label^ labelErrorTitle;
	private: System::Windows::Forms::Panel^ panelDebug;




	private: System::Windows::Forms::Panel^ panelDebugColor;



	private: System::Windows::Forms::Label^ labelDebugColor;

	private: System::Windows::Forms::Panel^ panelDebugPos;
	private: System::Windows::Forms::TextBox^ textBoxDebugPosX;
	private: System::Windows::Forms::TextBox^ textBoxDebugPosY;




	private: System::Windows::Forms::Label^ labelDebugPos;

	private: System::Windows::Forms::Label^ DebugTitle;
	private: System::Windows::Forms::Button^ buttonStopDebug;
	private: System::Windows::Forms::Button^ buttonLaunchDebug;

	private: System::Windows::Forms::ColorDialog^ colorDialogDebug;
	private: System::Windows::Forms::Button^ buttonDebugColor;




	private: System::Windows::Forms::Label^ labelMonitors;

	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: ajoutez ici le code du constructeur
			//
		}

	protected:
		/// <summary>
		/// Nettoyage des ressources utilis�es.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public:
		
	private:
		GLFWmonitor** listMonitors;
		int* nbMonitors = new int();
		System::String^ selectedMonitor;
		int* listLayer = NULL;
		System::String^ simFile;
		System::Threading::Thread^ simulation;
		static System::Windows::Forms::Label^ staticLabelError;
		static System::Windows::Forms::Button^ staticButtonLaunchSim;
		static System::Windows::Forms::Button^ staticButtonStopSim;
		array<System::Drawing::Color^>^ listColorDebug;

		// Initialize k components with k = # of monitors
		void InitializeComponentsWithMonitors() {
			this->listMonitorsLabel->Items->Clear();
			this->comboBoxLayer->Items->Clear();
			if (!glfwInit()) {
				//Handle Error
				this->labelError->Text = "Unable to initialize GLFW.";
				return;
			}
			this->listMonitors = glfwGetMonitors(nbMonitors);

			if (listLayer != NULL)
				delete listLayer;
			listLayer = new int(*nbMonitors);

			listColorDebug = gcnew array<System::Drawing::Color^> (*nbMonitors);

			for (int i = 0; i < *nbMonitors; i++) {
				const char* nameDisplay = glfwGetMonitorName(listMonitors[i]);
				System::Object^ nameDisplayString = System::String::Concat(gcnew array<System::String^>(3) {gcnew System::String(nameDisplay)," | ", gcnew System::String((i+1).ToString())});
				this->listMonitorsLabel->Items->Add(nameDisplayString);
			
				this->comboBoxLayer->Items->Add((i-1).ToString());

				listLayer[i] = NULL;

				listColorDebug[i] = gcnew System::Drawing::Color();
				listColorDebug[i]->FromName("White");
			}
			if (*nbMonitors != 0) {
				this->selectedMonitor = (System::String^) listMonitorsLabel->Items[0];
				this->UpdatePanelMonitor();
			}
		};

		void UpdatePanelMonitor() {
			int indexOfSelected = this->listMonitorsLabel->Items->IndexOf(this->selectedMonitor);
			if (indexOfSelected < 0 || indexOfSelected >= *nbMonitors) {
				//Handle Error
				this->labelError->Text = "Unable to find selected Monitor.";
				return;
			}
			this->labelInformationTitle->Text = this->selectedMonitor->ToString();
			int xpos, ypos;
			glfwGetMonitorPos(*(listMonitors + indexOfSelected), &xpos, &ypos);
			this->textBoxPosX->Text = gcnew System::String(xpos.ToString());
			this->textBoxPosY->Text = gcnew System::String(ypos.ToString());
			if (*(listMonitors + indexOfSelected) == glfwGetPrimaryMonitor()) {
				this->textBoxPrimaryMonitor->Text = "True";
			}
			else {
				this->textBoxPrimaryMonitor->Text = "False";
			}
			if (listLayer[indexOfSelected] != NULL)
				this->comboBoxLayer->SelectedItem = this->comboBoxLayer->Items[listLayer[indexOfSelected]];
			else
				this->comboBoxLayer->SelectedItem = NULL;

			this->buttonDebugColor->BackColor = *(this->listColorDebug[indexOfSelected]);
		}

		static void LaunchSim(Object^ pathFile) {
			msclr::interop::marshal_context converter;
			try {
				Simulator^ simulator = gcnew Simulator((char*)(converter.marshal_as<const char*>((System::String^)pathFile)));
				simulator->launchSim();
			}
			catch (std::exception e) {
				printErrorDelegate^ action = gcnew printErrorDelegate(&MainForm::printError);
				MainForm::staticLabelError->Invoke(action, gcnew array<Object^> {gcnew System::String(e.what())});
			}
			displayButtonLaunchSimDelegate^ actionLaunchSim = gcnew displayButtonLaunchSimDelegate(&MainForm::displayButtonLaunchSim);
			MainForm::staticButtonLaunchSim->Invoke(actionLaunchSim, gcnew array<Object^> {System::Boolean(true)});
			displayButtonStopSimDelegate^ actionStopSim = gcnew displayButtonStopSimDelegate(&MainForm::displayButtonStopSim);
			MainForm::staticButtonStopSim->Invoke(actionStopSim, gcnew array<Object^> {System::Boolean(true)});
		}

		delegate void displayButtonLaunchSimDelegate(System::Boolean launched);
		delegate void displayButtonStopSimDelegate(System::Boolean launched);
		delegate void printErrorDelegate(System::String^ msg);

		static void displayButtonLaunchSim(System::Boolean launched) {
			MainForm::staticButtonLaunchSim->Visible = launched;
		}

		static void displayButtonStopSim(System::Boolean launched) {
			MainForm::staticButtonStopSim->Visible = !launched;
		}

		static void printError(System::String^ msg) {
			MainForm::staticLabelError->Text = msg;
		}

		void InitSim(){
			this->simulation = gcnew System::Threading::Thread(gcnew System::Threading::ParameterizedThreadStart(&MainForm::LaunchSim));
			this->simulation->Start(this->textBoxFileProp->Text);
		}

		void finishSim() {
			this->simulation->Abort();
		}

		void changeColorDebug() {
			this->listColorDebug[this->listMonitorsLabel->Items->IndexOf(this->selectedMonitor)] = this->colorDialogDebug->Color;
		}


#pragma region Windows Form Designer generated code
		/// <summary>
		/// M�thode requise pour la prise en charge du concepteur - ne modifiez pas
		/// le contenu de cette m�thode avec l'�diteur de code.
		/// </summary>
		void InitializeComponent(void)
		{
			this->listMonitorsLabel = (gcnew System::Windows::Forms::ListBox());
			this->labelMonitors = (gcnew System::Windows::Forms::Label());
			this->menuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->fichierToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->quitterToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simulatorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->launchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->propertiesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->howToUseItToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->buttonAutoDetectMonitors = (gcnew System::Windows::Forms::Button());
			this->panelHandleMonitor = (gcnew System::Windows::Forms::Panel());
			this->panelDebug = (gcnew System::Windows::Forms::Panel());
			this->buttonStopDebug = (gcnew System::Windows::Forms::Button());
			this->buttonLaunchDebug = (gcnew System::Windows::Forms::Button());
			this->panelDebugColor = (gcnew System::Windows::Forms::Panel());
			this->labelDebugColor = (gcnew System::Windows::Forms::Label());
			this->panelDebugPos = (gcnew System::Windows::Forms::Panel());
			this->textBoxDebugPosX = (gcnew System::Windows::Forms::TextBox());
			this->textBoxDebugPosY = (gcnew System::Windows::Forms::TextBox());
			this->labelDebugPos = (gcnew System::Windows::Forms::Label());
			this->DebugTitle = (gcnew System::Windows::Forms::Label());
			this->panelInformation = (gcnew System::Windows::Forms::Panel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->comboBoxLayer = (gcnew System::Windows::Forms::ComboBox());
			this->labelLayerInfo = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->textBoxPrimaryMonitor = (gcnew System::Windows::Forms::TextBox());
			this->labelPrimaryMonitor = (gcnew System::Windows::Forms::Label());
			this->panelPosition = (gcnew System::Windows::Forms::Panel());
			this->textBoxPosX = (gcnew System::Windows::Forms::TextBox());
			this->textBoxPosY = (gcnew System::Windows::Forms::TextBox());
			this->labelPosition = (gcnew System::Windows::Forms::Label());
			this->labelInformation = (gcnew System::Windows::Forms::Label());
			this->buttonDetect = (gcnew System::Windows::Forms::Button());
			this->panelError = (gcnew System::Windows::Forms::Panel());
			this->labelErrorTitle = (gcnew System::Windows::Forms::Label());
			this->labelError = (gcnew System::Windows::Forms::Label());
			this->labelInformationTitle = (gcnew System::Windows::Forms::Label());
			this->panelSimulator = (gcnew System::Windows::Forms::Panel());
			this->buttonStopSimulation = (gcnew System::Windows::Forms::Button());
			this->buttonLaunchSim = (gcnew System::Windows::Forms::Button());
			this->buttonSelectFileProp = (gcnew System::Windows::Forms::Button());
			this->labelFilePropTitle = (gcnew System::Windows::Forms::Label());
			this->textBoxFileProp = (gcnew System::Windows::Forms::TextBox());
			this->labelSimTitle = (gcnew System::Windows::Forms::Label());
			this->openFileDialogFP = (gcnew System::Windows::Forms::OpenFileDialog());
			this->colorDialogDebug = (gcnew System::Windows::Forms::ColorDialog());
			this->buttonDebugColor = (gcnew System::Windows::Forms::Button());
			this->menuStrip->SuspendLayout();
			this->panelHandleMonitor->SuspendLayout();
			this->panelDebug->SuspendLayout();
			this->panelDebugColor->SuspendLayout();
			this->panelDebugPos->SuspendLayout();
			this->panelInformation->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel1->SuspendLayout();
			this->panelPosition->SuspendLayout();
			this->panelError->SuspendLayout();
			this->panelSimulator->SuspendLayout();
			this->SuspendLayout();
			// 
			// listMonitorsLabel
			// 
			this->listMonitorsLabel->FormattingEnabled = true;
			this->listMonitorsLabel->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Item1", L"Item2" });
			this->listMonitorsLabel->Location = System::Drawing::Point(12, 55);
			this->listMonitorsLabel->Name = L"listMonitorsLabel";
			this->listMonitorsLabel->Size = System::Drawing::Size(247, 108);
			this->listMonitorsLabel->TabIndex = 2;
			this->listMonitorsLabel->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::listMonitors_SelectedIndexChanged);
			// 
			// labelMonitors
			// 
			this->labelMonitors->AutoSize = true;
			this->labelMonitors->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->labelMonitors->Location = System::Drawing::Point(12, 32);
			this->labelMonitors->Name = L"labelMonitors";
			this->labelMonitors->Size = System::Drawing::Size(74, 20);
			this->labelMonitors->TabIndex = 3;
			this->labelMonitors->Text = L"Monitors";
			this->labelMonitors->Click += gcnew System::EventHandler(this, &MainForm::label1_Click);
			// 
			// menuStrip
			// 
			this->menuStrip->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->fichierToolStripMenuItem,
					this->simulatorToolStripMenuItem, this->helpToolStripMenuItem
			});
			this->menuStrip->Location = System::Drawing::Point(0, 0);
			this->menuStrip->Name = L"menuStrip";
			this->menuStrip->Size = System::Drawing::Size(176, 24);
			this->menuStrip->TabIndex = 5;
			this->menuStrip->Text = L"menuStrip1";
			// 
			// fichierToolStripMenuItem
			// 
			this->fichierToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->quitterToolStripMenuItem });
			this->fichierToolStripMenuItem->Name = L"fichierToolStripMenuItem";
			this->fichierToolStripMenuItem->Size = System::Drawing::Size(54, 20);
			this->fichierToolStripMenuItem->Text = L"Fichier";
			// 
			// quitterToolStripMenuItem
			// 
			this->quitterToolStripMenuItem->Name = L"quitterToolStripMenuItem";
			this->quitterToolStripMenuItem->Size = System::Drawing::Size(111, 22);
			this->quitterToolStripMenuItem->Text = L"Quitter";
			this->quitterToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::quitterToolStripMenuItem_Click);
			// 
			// simulatorToolStripMenuItem
			// 
			this->simulatorToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->launchToolStripMenuItem,
					this->propertiesToolStripMenuItem
			});
			this->simulatorToolStripMenuItem->Name = L"simulatorToolStripMenuItem";
			this->simulatorToolStripMenuItem->Size = System::Drawing::Size(70, 20);
			this->simulatorToolStripMenuItem->Text = L"Simulator";
			// 
			// launchToolStripMenuItem
			// 
			this->launchToolStripMenuItem->Name = L"launchToolStripMenuItem";
			this->launchToolStripMenuItem->Size = System::Drawing::Size(127, 22);
			this->launchToolStripMenuItem->Text = L"Launch";
			// 
			// propertiesToolStripMenuItem
			// 
			this->propertiesToolStripMenuItem->Name = L"propertiesToolStripMenuItem";
			this->propertiesToolStripMenuItem->Size = System::Drawing::Size(127, 22);
			this->propertiesToolStripMenuItem->Text = L"Properties";
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->howToUseItToolStripMenuItem,
					this->aboutToolStripMenuItem
			});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// howToUseItToolStripMenuItem
			// 
			this->howToUseItToolStripMenuItem->Name = L"howToUseItToolStripMenuItem";
			this->howToUseItToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->howToUseItToolStripMenuItem->Text = L"How to use it \?";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->aboutToolStripMenuItem->Text = L"About";
			// 
			// buttonAutoDetectMonitors
			// 
			this->buttonAutoDetectMonitors->Location = System::Drawing::Point(136, 170);
			this->buttonAutoDetectMonitors->Name = L"buttonAutoDetectMonitors";
			this->buttonAutoDetectMonitors->Size = System::Drawing::Size(123, 23);
			this->buttonAutoDetectMonitors->TabIndex = 6;
			this->buttonAutoDetectMonitors->Text = L"Auto-detect";
			this->buttonAutoDetectMonitors->UseVisualStyleBackColor = true;
			this->buttonAutoDetectMonitors->Click += gcnew System::EventHandler(this, &MainForm::buttonAutoDetectMonitors_Click);
			// 
			// panelHandleMonitor
			// 
			this->panelHandleMonitor->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panelHandleMonitor->Controls->Add(this->panelDebug);
			this->panelHandleMonitor->Controls->Add(this->panelInformation);
			this->panelHandleMonitor->Location = System::Drawing::Point(332, 36);
			this->panelHandleMonitor->Name = L"panelHandleMonitor";
			this->panelHandleMonitor->Size = System::Drawing::Size(399, 415);
			this->panelHandleMonitor->TabIndex = 7;
			// 
			// panelDebug
			// 
			this->panelDebug->Controls->Add(this->buttonStopDebug);
			this->panelDebug->Controls->Add(this->buttonLaunchDebug);
			this->panelDebug->Controls->Add(this->panelDebugColor);
			this->panelDebug->Controls->Add(this->panelDebugPos);
			this->panelDebug->Controls->Add(this->DebugTitle);
			this->panelDebug->Location = System::Drawing::Point(3, 108);
			this->panelDebug->Name = L"panelDebug";
			this->panelDebug->Size = System::Drawing::Size(393, 99);
			this->panelDebug->TabIndex = 5;
			// 
			// buttonStopDebug
			// 
			this->buttonStopDebug->BackColor = System::Drawing::Color::DarkRed;
			this->buttonStopDebug->Location = System::Drawing::Point(227, 56);
			this->buttonStopDebug->Name = L"buttonStopDebug";
			this->buttonStopDebug->Size = System::Drawing::Size(97, 25);
			this->buttonStopDebug->TabIndex = 6;
			this->buttonStopDebug->Text = L"Stop debug";
			this->buttonStopDebug->UseVisualStyleBackColor = false;
			this->buttonStopDebug->Visible = false;
			// 
			// buttonLaunchDebug
			// 
			this->buttonLaunchDebug->Location = System::Drawing::Point(227, 26);
			this->buttonLaunchDebug->Name = L"buttonLaunchDebug";
			this->buttonLaunchDebug->Size = System::Drawing::Size(97, 25);
			this->buttonLaunchDebug->TabIndex = 5;
			this->buttonLaunchDebug->Text = L"Launch debug";
			this->buttonLaunchDebug->UseVisualStyleBackColor = true;
			// 
			// panelDebugColor
			// 
			this->panelDebugColor->Controls->Add(this->buttonDebugColor);
			this->panelDebugColor->Controls->Add(this->labelDebugColor);
			this->panelDebugColor->Location = System::Drawing::Point(8, 57);
			this->panelDebugColor->Name = L"panelDebugColor";
			this->panelDebugColor->Size = System::Drawing::Size(155, 24);
			this->panelDebugColor->TabIndex = 4;
			// 
			// labelDebugColor
			// 
			this->labelDebugColor->AutoSize = true;
			this->labelDebugColor->Location = System::Drawing::Point(4, 3);
			this->labelDebugColor->Name = L"labelDebugColor";
			this->labelDebugColor->Size = System::Drawing::Size(81, 13);
			this->labelDebugColor->TabIndex = 1;
			this->labelDebugColor->Text = L"Color to display:";
			// 
			// panelDebugPos
			// 
			this->panelDebugPos->Controls->Add(this->textBoxDebugPosX);
			this->panelDebugPos->Controls->Add(this->textBoxDebugPosY);
			this->panelDebugPos->Controls->Add(this->labelDebugPos);
			this->panelDebugPos->Location = System::Drawing::Point(8, 27);
			this->panelDebugPos->Name = L"panelDebugPos";
			this->panelDebugPos->Size = System::Drawing::Size(155, 24);
			this->panelDebugPos->TabIndex = 2;
			// 
			// textBoxDebugPosX
			// 
			this->textBoxDebugPosX->Location = System::Drawing::Point(62, 0);
			this->textBoxDebugPosX->Name = L"textBoxDebugPosX";
			this->textBoxDebugPosX->Size = System::Drawing::Size(37, 20);
			this->textBoxDebugPosX->TabIndex = 3;
			// 
			// textBoxDebugPosY
			// 
			this->textBoxDebugPosY->Location = System::Drawing::Point(105, 0);
			this->textBoxDebugPosY->Name = L"textBoxDebugPosY";
			this->textBoxDebugPosY->Size = System::Drawing::Size(37, 20);
			this->textBoxDebugPosY->TabIndex = 2;
			// 
			// labelDebugPos
			// 
			this->labelDebugPos->AutoSize = true;
			this->labelDebugPos->Location = System::Drawing::Point(4, 3);
			this->labelDebugPos->Name = L"labelDebugPos";
			this->labelDebugPos->Size = System::Drawing::Size(50, 13);
			this->labelDebugPos->TabIndex = 1;
			this->labelDebugPos->Text = L"Position: ";
			// 
			// DebugTitle
			// 
			this->DebugTitle->AutoSize = true;
			this->DebugTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->DebugTitle->Location = System::Drawing::Point(4, 4);
			this->DebugTitle->Name = L"DebugTitle";
			this->DebugTitle->Size = System::Drawing::Size(58, 20);
			this->DebugTitle->TabIndex = 0;
			this->DebugTitle->Text = L"Debug";
			this->DebugTitle->Click += gcnew System::EventHandler(this, &MainForm::label4_Click);
			// 
			// panelInformation
			// 
			this->panelInformation->Controls->Add(this->panel2);
			this->panelInformation->Controls->Add(this->panel1);
			this->panelInformation->Controls->Add(this->panelPosition);
			this->panelInformation->Controls->Add(this->labelInformation);
			this->panelInformation->Location = System::Drawing::Point(3, 3);
			this->panelInformation->Name = L"panelInformation";
			this->panelInformation->Size = System::Drawing::Size(393, 99);
			this->panelInformation->TabIndex = 0;
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->comboBoxLayer);
			this->panel2->Controls->Add(this->labelLayerInfo);
			this->panel2->Location = System::Drawing::Point(180, 27);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(141, 30);
			this->panel2->TabIndex = 4;
			// 
			// comboBoxLayer
			// 
			this->comboBoxLayer->FormattingEnabled = true;
			this->comboBoxLayer->Location = System::Drawing::Point(47, 4);
			this->comboBoxLayer->Name = L"comboBoxLayer";
			this->comboBoxLayer->Size = System::Drawing::Size(83, 21);
			this->comboBoxLayer->TabIndex = 2;
			this->comboBoxLayer->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::comboBoxLayer_SelectedIndexChanged);
			// 
			// labelLayerInfo
			// 
			this->labelLayerInfo->AutoSize = true;
			this->labelLayerInfo->Location = System::Drawing::Point(5, 7);
			this->labelLayerInfo->Name = L"labelLayerInfo";
			this->labelLayerInfo->Size = System::Drawing::Size(36, 13);
			this->labelLayerInfo->TabIndex = 1;
			this->labelLayerInfo->Text = L"Layer:";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->textBoxPrimaryMonitor);
			this->panel1->Controls->Add(this->labelPrimaryMonitor);
			this->panel1->Location = System::Drawing::Point(8, 57);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(155, 24);
			this->panel1->TabIndex = 4;
			// 
			// textBoxPrimaryMonitor
			// 
			this->textBoxPrimaryMonitor->Location = System::Drawing::Point(105, 0);
			this->textBoxPrimaryMonitor->Name = L"textBoxPrimaryMonitor";
			this->textBoxPrimaryMonitor->ReadOnly = true;
			this->textBoxPrimaryMonitor->Size = System::Drawing::Size(37, 20);
			this->textBoxPrimaryMonitor->TabIndex = 3;
			// 
			// labelPrimaryMonitor
			// 
			this->labelPrimaryMonitor->AutoSize = true;
			this->labelPrimaryMonitor->Location = System::Drawing::Point(4, 3);
			this->labelPrimaryMonitor->Name = L"labelPrimaryMonitor";
			this->labelPrimaryMonitor->Size = System::Drawing::Size(82, 13);
			this->labelPrimaryMonitor->TabIndex = 1;
			this->labelPrimaryMonitor->Text = L"Primary Monitor:";
			// 
			// panelPosition
			// 
			this->panelPosition->Controls->Add(this->textBoxPosX);
			this->panelPosition->Controls->Add(this->textBoxPosY);
			this->panelPosition->Controls->Add(this->labelPosition);
			this->panelPosition->Location = System::Drawing::Point(8, 27);
			this->panelPosition->Name = L"panelPosition";
			this->panelPosition->Size = System::Drawing::Size(155, 24);
			this->panelPosition->TabIndex = 2;
			// 
			// textBoxPosX
			// 
			this->textBoxPosX->Location = System::Drawing::Point(62, 0);
			this->textBoxPosX->Name = L"textBoxPosX";
			this->textBoxPosX->ReadOnly = true;
			this->textBoxPosX->Size = System::Drawing::Size(37, 20);
			this->textBoxPosX->TabIndex = 3;
			this->textBoxPosX->TextChanged += gcnew System::EventHandler(this, &MainForm::textBoxPosX_TextChanged);
			// 
			// textBoxPosY
			// 
			this->textBoxPosY->Location = System::Drawing::Point(105, 0);
			this->textBoxPosY->Name = L"textBoxPosY";
			this->textBoxPosY->ReadOnly = true;
			this->textBoxPosY->Size = System::Drawing::Size(37, 20);
			this->textBoxPosY->TabIndex = 2;
			// 
			// labelPosition
			// 
			this->labelPosition->AutoSize = true;
			this->labelPosition->Location = System::Drawing::Point(4, 3);
			this->labelPosition->Name = L"labelPosition";
			this->labelPosition->Size = System::Drawing::Size(50, 13);
			this->labelPosition->TabIndex = 1;
			this->labelPosition->Text = L"Position: ";
			// 
			// labelInformation
			// 
			this->labelInformation->AutoSize = true;
			this->labelInformation->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->labelInformation->Location = System::Drawing::Point(4, 4);
			this->labelInformation->Name = L"labelInformation";
			this->labelInformation->Size = System::Drawing::Size(101, 20);
			this->labelInformation->TabIndex = 0;
			this->labelInformation->Text = L"Informations";
			// 
			// buttonDetect
			// 
			this->buttonDetect->Location = System::Drawing::Point(12, 170);
			this->buttonDetect->Name = L"buttonDetect";
			this->buttonDetect->Size = System::Drawing::Size(118, 23);
			this->buttonDetect->TabIndex = 8;
			this->buttonDetect->Text = L"Detect";
			this->buttonDetect->UseVisualStyleBackColor = true;
			this->buttonDetect->Click += gcnew System::EventHandler(this, &MainForm::buttonDetect_Click);
			// 
			// panelError
			// 
			this->panelError->Controls->Add(this->labelErrorTitle);
			this->panelError->Controls->Add(this->labelError);
			this->panelError->Location = System::Drawing::Point(12, 462);
			this->panelError->Name = L"panelError";
			this->panelError->Size = System::Drawing::Size(719, 55);
			this->panelError->TabIndex = 9;
			this->panelError->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainForm::panelError_Paint);
			// 
			// labelErrorTitle
			// 
			this->labelErrorTitle->AutoSize = true;
			this->labelErrorTitle->Location = System::Drawing::Point(7, 4);
			this->labelErrorTitle->Name = L"labelErrorTitle";
			this->labelErrorTitle->Size = System::Drawing::Size(35, 13);
			this->labelErrorTitle->TabIndex = 1;
			this->labelErrorTitle->Text = L"Error: ";
			// 
			// labelError
			// 
			this->labelError->AutoSize = true;
			this->labelError->ForeColor = System::Drawing::Color::DarkRed;
			this->labelError->Location = System::Drawing::Point(42, 4);
			this->labelError->Name = L"labelError";
			this->labelError->Size = System::Drawing::Size(0, 13);
			this->labelError->TabIndex = 0;
			// 
			// labelInformationTitle
			// 
			this->labelInformationTitle->AutoSize = true;
			this->labelInformationTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->labelInformationTitle->Location = System::Drawing::Point(328, 9);
			this->labelInformationTitle->Name = L"labelInformationTitle";
			this->labelInformationTitle->Size = System::Drawing::Size(91, 20);
			this->labelInformationTitle->TabIndex = 10;
			this->labelInformationTitle->Text = L"No Monitor";
			// 
			// panelSimulator
			// 
			this->panelSimulator->Controls->Add(this->buttonStopSimulation);
			this->panelSimulator->Controls->Add(this->buttonLaunchSim);
			this->panelSimulator->Controls->Add(this->buttonSelectFileProp);
			this->panelSimulator->Controls->Add(this->labelFilePropTitle);
			this->panelSimulator->Controls->Add(this->textBoxFileProp);
			this->panelSimulator->Controls->Add(this->labelSimTitle);
			this->panelSimulator->Location = System::Drawing::Point(11, 227);
			this->panelSimulator->Name = L"panelSimulator";
			this->panelSimulator->Size = System::Drawing::Size(247, 224);
			this->panelSimulator->TabIndex = 11;
			// 
			// buttonStopSimulation
			// 
			this->buttonStopSimulation->BackColor = System::Drawing::Color::Brown;
			this->buttonStopSimulation->Location = System::Drawing::Point(168, 198);
			this->buttonStopSimulation->Name = L"buttonStopSimulation";
			this->buttonStopSimulation->Size = System::Drawing::Size(75, 23);
			this->buttonStopSimulation->TabIndex = 17;
			this->buttonStopSimulation->Text = L"Stop Sim";
			this->buttonStopSimulation->UseVisualStyleBackColor = false;
			this->buttonStopSimulation->Visible = false;
			this->buttonStopSimulation->Click += gcnew System::EventHandler(this, &MainForm::buttonStopSimulation_Click);
			// 
			// buttonLaunchSim
			// 
			this->buttonLaunchSim->Location = System::Drawing::Point(8, 198);
			this->buttonLaunchSim->Name = L"buttonLaunchSim";
			this->buttonLaunchSim->Size = System::Drawing::Size(75, 23);
			this->buttonLaunchSim->TabIndex = 16;
			this->buttonLaunchSim->Text = L"Launch Sim";
			this->buttonLaunchSim->UseVisualStyleBackColor = true;
			this->buttonLaunchSim->Click += gcnew System::EventHandler(this, &MainForm::buttonLaunchSim_Click);
			// 
			// buttonSelectFileProp
			// 
			this->buttonSelectFileProp->Location = System::Drawing::Point(202, 36);
			this->buttonSelectFileProp->Name = L"buttonSelectFileProp";
			this->buttonSelectFileProp->Size = System::Drawing::Size(41, 19);
			this->buttonSelectFileProp->TabIndex = 15;
			this->buttonSelectFileProp->Text = L". . .";
			this->buttonSelectFileProp->UseVisualStyleBackColor = true;
			this->buttonSelectFileProp->Click += gcnew System::EventHandler(this, &MainForm::buttonSelectFileProp_Click);
			// 
			// labelFilePropTitle
			// 
			this->labelFilePropTitle->AutoSize = true;
			this->labelFilePropTitle->Location = System::Drawing::Point(5, 20);
			this->labelFilePropTitle->Name = L"labelFilePropTitle";
			this->labelFilePropTitle->Size = System::Drawing::Size(65, 13);
			this->labelFilePropTitle->TabIndex = 14;
			this->labelFilePropTitle->Text = L"File Property";
			// 
			// textBoxFileProp
			// 
			this->textBoxFileProp->Location = System::Drawing::Point(5, 36);
			this->textBoxFileProp->Name = L"textBoxFileProp";
			this->textBoxFileProp->Size = System::Drawing::Size(191, 20);
			this->textBoxFileProp->TabIndex = 13;
			// 
			// labelSimTitle
			// 
			this->labelSimTitle->AutoSize = true;
			this->labelSimTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F));
			this->labelSimTitle->Location = System::Drawing::Point(4, 0);
			this->labelSimTitle->Name = L"labelSimTitle";
			this->labelSimTitle->Size = System::Drawing::Size(80, 20);
			this->labelSimTitle->TabIndex = 12;
			this->labelSimTitle->Text = L"Simulator";
			// 
			// openFileDialogFP
			// 
			this->openFileDialogFP->Title = L"Chooose property file";
			// 
			// colorDialogDebug
			// 
			this->colorDialogDebug->AnyColor = true;
			// 
			// buttonDebugColor
			// 
			this->buttonDebugColor->BackColor = System::Drawing::Color::White;
			this->buttonDebugColor->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->buttonDebugColor->Location = System::Drawing::Point(88, 1);
			this->buttonDebugColor->Name = L"buttonDebugColor";
			this->buttonDebugColor->Size = System::Drawing::Size(64, 23);
			this->buttonDebugColor->TabIndex = 4;
			this->buttonDebugColor->UseVisualStyleBackColor = false;
			this->buttonDebugColor->Click += gcnew System::EventHandler(this, &MainForm::buttonDebugColor_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(743, 613);
			this->Controls->Add(this->panelSimulator);
			this->Controls->Add(this->labelInformationTitle);
			this->Controls->Add(this->panelError);
			this->Controls->Add(this->buttonDetect);
			this->Controls->Add(this->panelHandleMonitor);
			this->Controls->Add(this->buttonAutoDetectMonitors);
			this->Controls->Add(this->labelMonitors);
			this->Controls->Add(this->listMonitorsLabel);
			this->Controls->Add(this->menuStrip);
			this->MainMenuStrip = this->menuStrip;
			this->Name = L"MainForm";
			this->Text = L"TensorDisplay Software";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->menuStrip->ResumeLayout(false);
			this->menuStrip->PerformLayout();
			this->panelHandleMonitor->ResumeLayout(false);
			this->panelDebug->ResumeLayout(false);
			this->panelDebug->PerformLayout();
			this->panelDebugColor->ResumeLayout(false);
			this->panelDebugColor->PerformLayout();
			this->panelDebugPos->ResumeLayout(false);
			this->panelDebugPos->PerformLayout();
			this->panelInformation->ResumeLayout(false);
			this->panelInformation->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->panelPosition->ResumeLayout(false);
			this->panelPosition->PerformLayout();
			this->panelError->ResumeLayout(false);
			this->panelError->PerformLayout();
			this->panelSimulator->ResumeLayout(false);
			this->panelSimulator->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
		this->staticButtonLaunchSim = this->buttonLaunchSim;
		this->staticButtonStopSim = this->buttonStopSimulation;
		this->staticLabelError = this->labelError;
		this->InitializeComponentsWithMonitors();
	}
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void quitterToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		this->~MainForm();
	}
private: System::Void buttonAutoDetectMonitors_Click(System::Object^ sender, System::EventArgs^ e) {
	
}
private: System::Void listMonitors_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	if (this->listMonitorsLabel->SelectedItem != nullptr) {
		this->selectedMonitor = (System::String^)this->listMonitorsLabel->SelectedItem;
		this->UpdatePanelMonitor();
	}
}
private: System::Void buttonDetect_Click(System::Object^ sender, System::EventArgs^ e) {
	this->InitializeComponentsWithMonitors();
}
private: System::Void textBoxPosX_TextChanged(System::Object^ sender, System::EventArgs^ e) {

}
private: System::Void comboBoxLayer_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	listLayer[this->listMonitorsLabel->Items->IndexOf(this->selectedMonitor)] = System::Convert::ToInt32(comboBoxLayer->SelectedItem);
}
private: System::Void buttonSelectFileProp_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Windows::Forms::DialogResult result = this->openFileDialogFP->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK) {
		this->textBoxFileProp->Text = this->openFileDialogFP->FileName;
		this->simFile = this->openFileDialogFP->FileName;
	}
}
private: System::Void buttonLaunchSim_Click(System::Object^ sender, System::EventArgs^ e) {
	this->InitSim();
	this->buttonLaunchSim->Visible = false;
	this->buttonStopSimulation->Visible = true;
}
private: System::Void buttonStopSimulation_Click(System::Object^ sender, System::EventArgs^ e) {
	this->finishSim();
	this->buttonLaunchSim->Visible = true;
	this->buttonStopSimulation->Visible = false;
}
private: System::Void panelError_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
}
private: System::Void label4_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void buttonDebugColor_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Windows::Forms::DialogResult result = this->colorDialogDebug->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK) {
		this->buttonDebugColor->BackColor = this->colorDialogDebug->Color;
		if (this->selectedMonitor != nullptr) {
			this->changeColorDebug();
		}
	}
}
};
}
