#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    CurrentOperation = new Operation();
    ui->setupUi(this);
    hide_elements();
    //note and user name hide
    ui->NoteInput->setVisible(false);
    ui->UserNameInput->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    currentUTCtime = QDateTime::currentDateTimeUtc();
    pacificTimeZone = QTimeZone("America/Los_Angeles");
    qlogpath = QString::fromStdString(logpath);
}

MainWindow::~MainWindow()
{
    delete ui;
    CurrentOperation->reset();
    delete CurrentOperation;
    clear_vectors();
}

void MainWindow::hide_elements(){
    //upload manifest hide
    ui->Upload_Manifest_Confirm->setVisible(false);
    //xmoves hide
    ui->XMovesXminutestextdisplay->setText("Calculating . . .");
    ui->X_Moves_X_Minutes_Confirm->setVisible(false);
    //unload hide
    ui->waitlabel->setVisible(true);
    ui->unLoadContainerDisplay->setVisible(false);
    ui->Select_Unload_Confirm->setVisible(false);
    //download hide
    ui->downloadmanifestdisplay->setText("Please wait while manifest file is being saved . . .");
    ui->Download_Manifest_Confirm->setVisible(false);
    //weight hide
    ui->weightinput->setVisible(false);
    ui->weightprompt->setVisible(false);
}
void MainWindow::clear_vectors(){
    to_be_loaded.clear();
    to_be_unloaded.clear();
    to_be_unloaded_options.clear();
    to_be_completed_moves.clear();
}

void MainWindow::calculate(){
    CurrentOperation->get_containers();//needed for demo purposes only
    to_be_completed_moves = CurrentOperation->get_moves();
    time = 0;
    for(unsigned long long i = 0; i<to_be_completed_moves.size();i++){
        time+= to_be_completed_moves.at(i)->get_time();
    }
    string output = "This Operation will take "+ to_string(to_be_completed_moves.size())+" Moves and "+ to_string(time) +" Minutes.";
    ui->XMovesXminutestextdisplay->setText(QString::fromStdString(output));
    ui->X_Moves_X_Minutes_Confirm->setVisible(true);
}

void MainWindow::get_unload_options(){
    to_be_unloaded_options = CurrentOperation->get_containers();
    for(unsigned long long i = 0; i<to_be_unloaded_options.size();i++){
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromStdString(to_string(i+1)+" "+to_be_unloaded_options.at(i)->get_description()+". "+to_string(to_be_unloaded_options.at(i)->get_weight())+" Kilos."));
        item->setCheckState(Qt::Unchecked);
        ui->unLoadContainerDisplay->addItem(item);
    }
    ui->unLoadContainerDisplay->setVisible(true);
    ui->Select_Unload_Confirm->setVisible(true);
    ui->waitlabel->setVisible(false);
}

void MainWindow::display_move(unsigned long long i){
    ui->Step_X_of_X_Confirm->setVisible(false);
    Container* currentContainer = to_be_completed_moves.at(i)->get_container();
    string location = " ";
    string finalLocation = " ";
    if(currentContainer->get_location()=="t"){
        location = "Truck";
    }
    else if(currentContainer->get_location().at(0)=='b'){
        location = "Buffer at "+ currentContainer->get_location().substr(2,5);
    }
    else{
        location = "Ship at "+ currentContainer->get_location().substr(2,5);
    }

    if(to_be_completed_moves.at(i)->get_final_location()=="t"){
        finalLocation = "Truck";
    }
    else if(to_be_completed_moves.at(i)->get_final_location().at(0)=='b'){
        finalLocation = "Buffer at "+ to_be_completed_moves.at(i)->get_final_location().substr(2,5);
    }
    else{
        finalLocation = "Ship at "+ to_be_completed_moves.at(i)->get_final_location().substr(2,5);
    }
    moveoutput = "Step "+to_string(i+1)+" of "+ to_string(to_be_completed_moves.size())+ "\n";
    moveoutput+="Move "+currentContainer->get_description()+" from "+location+" to "+finalLocation+"\n";
    moveoutput+="Time for this move: "+to_string(to_be_completed_moves.at(i)->get_time())+" Minutes\n";
    moveoutput+="Time remaining: "+to_string(time)+" Minutes\n";
    ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
    if(currentContainer->get_weight()==-1){
        ui->weightinput->setVisible(true);
        ui->weightprompt->setVisible(true);
    }
    else{
        moveoutput+="Weight: "+to_string(currentContainer->get_weight())+" Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
    }

}

void MainWindow::save(){
    filepath = CurrentOperation->get_manifest_path();
    string output = "Manifest: "+filename+" has been saved to: "+filepath+"\n Please send to captain.";
    ui->downloadmanifestdisplay->setText(QString::fromStdString(output));
    ui->Download_Manifest_Confirm->setVisible(true);
}

string MainWindow::get_date_and_time(){
    currentTime = currentUTCtime.toTimeZone(pacificTimeZone);
    int year = currentTime.date().year();
    int month = currentTime.date().month();
    int day = currentTime.date().day();
    QString formattedDay = QString("%1").arg(day, 2, 10, QChar('0'));
    QString formattedMinutes = currentTime.toString("mm");
    QString formattedHours = currentTime.toString("hh");

    string log_date_time = to_string(month) + "/" + formattedDay.toStdString() + "/" + to_string(year) + ": " + formattedHours.toStdString() + ":" + formattedMinutes.toStdString() + " ";
    return log_date_time;
}

void MainWindow::updatelog(string description){
    QFile file (qlogpath);
    string datetime = get_date_and_time();
    string s = datetime + description;

    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        QTextStream stream(&file);
        stream << QString::fromStdString(s) << "\n";
        file.close();
    } else {
        qDebug() << "Error opening file:" << file.errorString();
    }
}

void MainWindow::on_Main_Menu_Load_Unload_clicked()
{
    load_or_balance = 'l';
    CurrentOperation->set_load_or_balance(load_or_balance);
    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindow::on_Main_Menu_Balance_clicked()
{
    load_or_balance = 'b';
    CurrentOperation->set_load_or_balance(load_or_balance);
    ui->stackedWidget->setCurrentIndex(6);
}

//upload manifest slots
void MainWindow::on_Upload_Manifest_Confirm_clicked()
{
    CurrentOperation->set_manifest_path(filepath);//sends file path to backend now that user has confirmed their choice
    if(load_or_balance=='l'){
        ui->stackedWidget->setCurrentIndex(2);
        get_unload_options();
        string description = "Manifest " + filename + " is opened, there are " + to_string(to_be_unloaded_options.size()) + " containers on the ship";
        updatelog(description);
    }
    else{
        ui->stackedWidget->setCurrentIndex(3);
        calculate();
    }
}

void MainWindow::on_UploadManifestSelectFile_clicked()
{
    QString qfilepath = QFileDialog::getOpenFileName(this,"Open Manifest",QDir::homePath(),"Text Files (*.txt)");
    filepath = qfilepath.toStdString();
    if(filepath!=""){
        QFileInfo fileInfo(qfilepath);
        QString qfilename = fileInfo.fileName();
        filename = qfilename.toStdString();
        ui->ManifestDisplay->setText(qfilename);
        ui->Upload_Manifest_Confirm->setVisible(true);
    }
}

//select unload slots
void MainWindow::on_Select_Unload_Confirm_clicked()
{
    for(unsigned long long i=0;i<indexVector.size();i++){
        to_be_unloaded.push_back(to_be_unloaded_options[static_cast<unsigned long long>(indexVector[i])]);
    }
    CurrentOperation->set_unload(to_be_unloaded);
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_unLoadContainerDisplay_itemClicked(QListWidgetItem *item)
{
    string description = item->text().toStdString();
    int index = int(description.at(0))-49;
    if(item->checkState()==Qt::Checked){
        item->setCheckState(Qt::Unchecked);
        indexVector.erase(remove(indexVector.begin(),indexVector.end(),index),indexVector.end());
    }
    else{
        item->setCheckState(Qt::Checked);
        indexVector.push_back(index);
    }
}
//select load slots
//TO DO: check for bad input
void MainWindow::on_LoadContainerInput_returnPressed()
{
    QString qcontainer = ui->LoadContainerInput->text();
    QListWidgetItem* item = new QListWidgetItem(qcontainer,ui->LoadContainerDisplay);
    ui->LoadContainerDisplay->addItem(item);
    string container = qcontainer.toStdString();
    //Container::Container(string l, string d, int w)
    to_be_loaded.push_back(new Container("t",container,-1));
    ui->LoadContainerInput->setText(" ");
}

void MainWindow::on_Select_Load_Confirm_clicked()
{
    CurrentOperation->set_load(to_be_loaded);
    ui->stackedWidget->setCurrentIndex(3);//move to x moves x minutes
    calculate();
}

//x moves x minutes DONE
void MainWindow::on_X_Moves_X_Minutes_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    display_move(index);
}

//step x of x slots
//TO DO check for valid input
void MainWindow::on_weightinput_returnPressed()
{
    Container* currentContainer = to_be_completed_moves.at(index)->get_container();
    string sweight= ui->weightinput->text().toStdString();
    int weight = stoi(sweight);
    currentContainer->set_weight(weight);
    moveoutput+="Weight: "+to_string(currentContainer->get_weight())+" Kilograms";
    ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
    ui->Step_X_of_X_Confirm->setVisible(true);
    ui->weightinput->setText(" ");
    ui->weightinput->setVisible(false);
    ui->weightprompt->setVisible(false);
}

void MainWindow::on_Step_X_of_X_Confirm_clicked()
{
    time-=to_be_completed_moves.at(index)->get_time();
    CurrentOperation->move_complete(index);
    index++;
    if(index == to_be_completed_moves.size()){
        ui->stackedWidget->setCurrentIndex(5);
        save();
    }
    else{
        display_move(index);
    }
}

//TO DO: reset everything
void MainWindow::on_Download_Manifest_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->ManifestDisplay->setText(" ");
    time = 0;
    index = 0;
    filepath = " ";
    filename = " ";
    load_or_balance = ' ';
    indexVector.clear();
    //clear_vectors();
    CurrentOperation->reset();
    hide_elements();
}

//Main window slots
//TO DO: check for valid input before passing to backend
void MainWindow::on_UserNameInput_returnPressed()
{
    QString qname = ui->UserNameInput->text();
    string name = qname.toStdString();
    ui->UserNameDisplay->setText(qname);
    CurrentOperation->set_username(name);
    ui->UserNameInput->setText(" ");
    ui->UserNameInput->setVisible(false);
    updatelog(name + " signs in");
}

void MainWindow::on_NoteInput_returnPressed()
{
    string note = ui->NoteInput->text().toStdString();
    CurrentOperation->set_note(note);
    ui->NoteInput->setText(" ");
    ui->NoteInput->setVisible(false);
    updatelog(note);
}

void MainWindow::on_Main_Window_Note_clicked()
{
    ui->NoteInput->setVisible(true);
}


void MainWindow::on_Main_Window_Sign_In_clicked()
{
    ui->UserNameInput->setVisible(true);
}


