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
    set_up_animation();
    currentUTCtime = QDateTime::currentDateTimeUtc();
    pacificTimeZone = QTimeZone("America/Los_Angeles");
    qlogpath = QString::fromStdString(logpath);
    flashTimer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    ui->LoadContainerDisplay->clear();
    ui->unLoadContainerDisplay->clear();
    delete ui;
    CurrentOperation->reset();
    delete CurrentOperation;
    clear_vectors();
}

// Helper Functions
void MainWindow::showDialog(const QString &message) {
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(140);
    dialog->setMinimumWidth(740);

    QLabel *displayInvalidStrings = new QLabel();
    displayInvalidStrings->setParent(dialog);
    QFont font("Segoe UI", 18);
    displayInvalidStrings->setFont(font);
    displayInvalidStrings->setGeometry(10, 10, 720, 100);
    displayInvalidStrings->setText(message);
    displayInvalidStrings->show();

    dialog->show();
}

bool MainWindow::containsNonPrintableCharacters(const QString &text) {
    static QRegularExpression nonPrintableRegex("[^\\x20-\\x7E]");
    return text.contains(nonPrintableRegex);
}

QString MainWindow::removeTxtExtension(const QString &filename)
{
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().toLower() == "txt") {
        // Remove ".txt" at the end
        return fileInfo.baseName();
    }
    return filename;
}

void MainWindow::hide_elements()
{
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

void MainWindow::clear_vectors()
{
    to_be_loaded.clear();
    to_be_unloaded.clear();
    to_be_unloaded_options.clear();
    to_be_completed_moves.clear();
}

void MainWindow::calculate()
{
    to_be_completed_moves = CurrentOperation->get_moves();
    time = 0;
    for (unsigned long long i = 0; i < to_be_completed_moves.size(); i++) {
        time += to_be_completed_moves.at(i)->get_time();
    }
    string output = "This Operation will take " + to_string(to_be_completed_moves.size()) + " Moves and " + to_string(time) + " Minutes.";
    ui->XMovesXminutestextdisplay->setText(QString::fromStdString(output));
    ui->X_Moves_X_Minutes_Confirm->setVisible(true);
}

void MainWindow::get_unload_options()
{
    to_be_unloaded_options = CurrentOperation->get_containers();
    for (unsigned long long i = 0; i < to_be_unloaded_options.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromStdString(to_string(i + 1) + " " + to_be_unloaded_options.at(i)->get_description() + ". " + to_string(to_be_unloaded_options.at(i)->get_weight()) + " Kilos."));
        item->setCheckState(Qt::Unchecked);
        ui->unLoadContainerDisplay->addItem(item);
    }
    ui->unLoadContainerDisplay->setVisible(true);
    ui->Select_Unload_Confirm->setVisible(true);
    ui->waitlabel->setVisible(false);
}

void MainWindow::display_move(unsigned long long i)
{
    ui->Step_X_of_X_Confirm->setVisible(false);
    disconnect(flashTimer, SIGNAL(timeout()), this, SLOT(toggleLabelVisibility()));
    Container *currentContainer = to_be_completed_moves.at(i)->get_container();
    string location = " ";
    string finalLocation = " ";
    string coord = "";
    string coordname = "";
    if (currentContainer->get_location() == "t") {
        location = "Truck";
        coordname = "truckborder";
    } else if (currentContainer->get_location().at(0) == 'b') {
        coord = currentContainer->get_location().substr(2, 5);
        location = "Buffer at " + coord;
        coordname = "b" + coord.substr(0,2) + coord.substr(3,2);
    } else {
        coord = currentContainer->get_location().substr(2, 5);
        location = "Ship at " + coord;
        coordname = "s" + coord.substr(0,2) + coord.substr(3,2);
    }
    set_container_style(QString::fromStdString(coordname), "chosen");
    CurrentOperation->set_current_container(QString::fromStdString(coordname));

    coord = "";
    coordname = "";

    if (to_be_completed_moves.at(i)->get_final_location() == "t") {
        finalLocation = "Truck";
        coordname = "truckborder";
    } else if (to_be_completed_moves.at(i)->get_final_location().at(0) == 'b') {
        coord = to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        finalLocation = "Buffer at " + coord;
        coordname = "b" + coord.substr(0,2) + coord.substr(3,2);
    } else {
        coord = to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        finalLocation = "Ship at " + to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        coordname = "s" + coord.substr(0,2) + coord.substr(3,2);
    }

    set_container_style(QString::fromStdString(coordname), "goal");
    CurrentOperation->set_goal_loc(QString::fromStdString(coordname));

    moveoutput = "Step " + to_string(i + 1) + " of " + to_string(to_be_completed_moves.size()) + "\n";
    moveoutput += "Move " + currentContainer->get_description() + " from " + location + " to " + finalLocation + "\n";
    moveoutput += "Time for this move: " + to_string(to_be_completed_moves.at(i)->get_time()) + " Minutes\n";
    moveoutput += "Time remaining: " + to_string(time) + " Minutes\n";
    ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
    if (currentContainer->get_weight() == -1) {
        ui->weightinput->setVisible(true);
        ui->weightprompt->setVisible(true);
    } else {
        moveoutput += "Weight: " + to_string(currentContainer->get_weight()) + " Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
    }
}

void MainWindow::save()
{
    filepath = CurrentOperation->get_manifest_path();
    string output = "Manifest: " + filename + "_OUTBOUND has been saved to:\n " + filepath ;
    showDialog("Please send outbound manifest to captain.");
    ui->downloadmanifestdisplay->setText(QString::fromStdString(output));
    ui->Download_Manifest_Confirm->setVisible(true);
}


//Flashing Display Slots:
void MainWindow::toggleLabelVisibility(){
    toggleLabelVisibility(CurrentOperation->get_goal_loc());
}
void MainWindow::toggleLabelVisibility(const QString labelName){
    QLabel *label = findChild<QLabel*>(labelName);
    if (label) {
        label->setVisible(!label->isVisible());
    } else {
        cout << "cannot find label" << endl;
    }
}

//Log Slots:
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

//Main Menu Slots:
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

//Upload Manifest Slots:
void MainWindow::on_Upload_Manifest_Confirm_clicked()
{
    CurrentOperation->set_manifest_path(filepath); //sends file path to backend now that user has confirmed their choice
    if (load_or_balance == 'l') {
        ui->stackedWidget->setCurrentIndex(2);
        get_unload_options();
        string description = "Manifest " + filename + " is opened, there are " + to_string(to_be_unloaded_options.size()) + " containers on the ship";
        updatelog(description);
        initial_container_setup();
    } else {
        ui->stackedWidget->setCurrentIndex(3);
        calculate();
    }
}

void MainWindow::on_UploadManifestSelectFile_clicked()
{
    QString qfilepath = QFileDialog::getOpenFileName(this, "Open Manifest",QDir::homePath(),"Text Files (*.txt)");
    filepath = qfilepath.toStdString();
    if (filepath != "") {
        QFileInfo fileInfo(qfilepath);
        QString qfilename = fileInfo.fileName();
        QString qfilenameWithoutTxt = removeTxtExtension(qfilename);
        filename = qfilenameWithoutTxt.toStdString();
        ui->ManifestDisplay->setText(qfilenameWithoutTxt);
        ui->Upload_Manifest_Confirm->setVisible(true);
    }
}

//Select Unload Slots
void MainWindow::on_Select_Unload_Confirm_clicked()
{
    for (unsigned long long i = 0; i < indexVector.size(); i++) {
        to_be_unloaded.push_back(
            to_be_unloaded_options[static_cast<unsigned long long>(indexVector[i])]);
    }
    CurrentOperation->set_unload(to_be_unloaded);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_unLoadContainerDisplay_itemClicked(QListWidgetItem *item)
{
    string description = item->text().toStdString();
    int index = int(description.at(0)) - 49;
    if (item->checkState() == Qt::Checked) {
        item->setCheckState(Qt::Unchecked);
        indexVector.erase(remove(indexVector.begin(), indexVector.end(), index), indexVector.end());
    } else {
        item->setCheckState(Qt::Checked);
        indexVector.push_back(index);
    }
}

//Select Load Slots:
void MainWindow::on_LoadContainerInput_returnPressed()
{
    QString qcontainer = ui->LoadContainerInput->text().trimmed();  // Trim whitespace

    // Check for an empty string or non printable string
    if (qcontainer.isEmpty()||containsNonPrintableCharacters(qcontainer)) {
        showDialog("Please enter at least one printable character.");
        ui->LoadContainerInput->setText("");
        return;
    }

    QStringList invalidStrings = {"nan", "unused", "empty"};
    for (const QString &forbidden : invalidStrings) {
        if (qcontainer.toLower() == forbidden) {
            // Handle invalid input
            showDialog("Your container description may not solely consist of the words:\n \"NAN\", \"Unused\", or \"Empty\".");
            ui->LoadContainerInput->setText("");
            return;
        }
    }

    // Valid input, add to the list and clear the input
    QListWidgetItem *item = new QListWidgetItem(qcontainer, ui->LoadContainerDisplay);
    ui->LoadContainerDisplay->addItem(item);
    string container = qcontainer.toStdString();
    to_be_loaded.push_back(new Container("t", container, -1));
    ui->LoadContainerInput->setText("");
}

void MainWindow::on_Select_Load_Confirm_clicked()
{
    CurrentOperation->set_load(to_be_loaded);
    ui->stackedWidget->setCurrentIndex(3);
    calculate();
}

//X Moves X Minutes Slots:
void MainWindow::on_X_Moves_X_Minutes_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    display_move(index);
}

//Step X of X Slots:
void MainWindow::on_weightinput_returnPressed()
{
    Container *currentContainer = to_be_completed_moves.at(index)->get_container();
    QString inputText = ui->weightinput->text();

    try {
        // Attempt to convert the input string to an integer
        int weight = inputText.toInt();

        // Check if the integer is within the valid range
        if (weight < 1 || weight > 99999) {
            ui->weightinput->setText("");
            showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
            return;
        }

        // Set the weight for the current container
        currentContainer->set_weight(weight);

        // Update UI and display information
        moveoutput += "Weight: " + to_string(currentContainer->get_weight()) + " Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
        ui->weightinput->setText("");
        ui->weightinput->setVisible(false);
        ui->weightprompt->setVisible(false);
    } catch (const std::invalid_argument &e) {
        // Handle invalid input (non-integer)
        showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
        ui->weightinput->setText("");
        return;
    } catch (const std::out_of_range &e) {
        // Handle out-of-range input
        showDialog("Please enter an integer weight within the range:\n 1 - 99999 kilos.");
        ui->weightinput->setText("");
        return;
    }
}

void MainWindow::on_Step_X_of_X_Confirm_clicked()
{
    time -= to_be_completed_moves.at(index)->get_time();
    CurrentOperation->move_complete(index);
    update_container_styles();
    index++;
    if (index == to_be_completed_moves.size()) {
        ui->stackedWidget->setCurrentIndex(5);
        save();
    } else {
        display_move(index);
    }
}

//Download Manifest Slots:
void MainWindow::on_Download_Manifest_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->ManifestDisplay->setText("");
    time = 0;
    index = 0;
    filepath = "";
    filename = "";
    load_or_balance = ' ';
    ui->LoadContainerDisplay->clear();
    ui->unLoadContainerDisplay->clear();
    indexVector.clear();
    CurrentOperation->reset();
    hide_elements();
    clear_vectors();
}

//Main Window Slots:
void MainWindow::on_UserNameInput_returnPressed()
{
    QString qname = ui->UserNameInput->text().trimmed();
    if (containsNonPrintableCharacters(qname)||qname.isEmpty()) {
        ui->UserNameInput->setText("");
        showDialog("Please enter at least one printable character.");
        return;
    }
    string name = qname.toStdString();
    ui->UserNameDisplay->setText(qname);
    CurrentOperation->set_username(name);
    ui->UserNameInput->setText("");
    ui->UserNameInput->setVisible(false);
    updatelog(name + " signs in");
}

void MainWindow::on_NoteInput_returnPressed()
{
    string note = ui->NoteInput->text().toStdString();
    CurrentOperation->set_note(note);
    ui->NoteInput->setText("");
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

//Set Up Animation
void MainWindow::set_up_animation(){
    ui->animationbase->setPixmap(QPixmap(":/images/animationbase.png"));
    QString reg_row = "background-color: white; border-left: 0.5px solid black; border-bottom: 0.5px solid black;";
    QString right = "background-color: white; border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-right: 0.5px solid black;";
    QString top_row = "background-color: white; border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-top: 0.5px solid black;";
    QString top_right = "background-color: white; border: 0.5px solid black;";
    QString temp = "background-color: white; border-left: 0.5px dashed gray; border-top: 0.5px dashed gray;";
    QString temp_right = "background-color: white; border-left: 0.5px dashed gray; border-top: 0.5px dashed gray; border-right: 0.5px dashed gray;";

    //Buffer rows
    for (int row = 1; row <= 4; ++row) {
        for (int col = 1; col <= 24; ++col) {
            QString labelName = QString("b%1%2").arg(row, 2, 10, QChar('0')).arg(col, 2, 10, QChar('0'));
            QLabel *currentLabel = dynamic_cast<QLabel*>(ui->centralwidget->findChild<QObject*>(labelName));

            if (currentLabel) {
                if( row == 4 && col == 24){
                    currentLabel->setStyleSheet(top_right);
                }else if (col == 24) {
                    currentLabel->setStyleSheet(right);
                } else  if (row == 4){
                    currentLabel->setStyleSheet(top_row);
                } else {
                    currentLabel->setStyleSheet(reg_row);
                }

            }
        }
    }

    //Ship rows
    for (int row = 1; row <= 9; ++row) {
        for (int col = 1; col <= 12; ++col) {
            QString labelName = QString("s%1%2").arg(row, 2, 10, QChar('0')).arg(col, 2, 10, QChar('0'));
            QLabel *currentLabel = dynamic_cast<QLabel*>(ui->centralwidget->findChild<QObject*>(labelName));

            if (currentLabel) {
                if( row == 9 ){
                    if(col == 12){
                        currentLabel->setStyleSheet(temp_right);
                    } else {
                        currentLabel->setStyleSheet(temp);
                    }
                } else if (row == 8 && col == 12){
                    currentLabel->setStyleSheet(top_right);
                } else if (col == 12) {
                    currentLabel->setStyleSheet(right);
                } else  if (row == 8){
                    currentLabel->setStyleSheet(top_row);
                } else {
                    currentLabel->setStyleSheet(reg_row);
                }
            }
        }
    }

    //Buffer coordinate labels
    ui->buffer_xlabel1->setStyleSheet("color: black;");
    ui->buffer_xlabel2->setStyleSheet("color: black;");
    ui->buffer_ylabel1->setStyleSheet("color: black;");
    ui->buffer_ylabel2->setStyleSheet("color: black;");
    ui->bufferlabel->setStyleSheet("color: black;");
    ui->bufferborder->setStyleSheet("border: 0.5px solid black;");
    //Ship coordinate labels
    ui->ship_xlabel1->setStyleSheet("color: black;");
    ui->ship_xlabel2->setStyleSheet("color: black;");
    ui->ship_ylabel1->setStyleSheet("color: black;");
    ui->ship_ylabel2->setStyleSheet("color: black;");
    ui->shipborder->setStyleSheet("border: 0.5px solid black;");
    ui->shiplabel->setStyleSheet("color: black;");

}

void MainWindow::set_container_style(const QString containerName, string type){
    cout << "set container style " << containerName.toStdString() << " " << type << endl;
    QLabel *label = dynamic_cast<QLabel*>(ui->centralwidget->findChild<QObject*>(containerName));
    QString border = "";
    if(containerName[0] != 't'){
        border = get_border_type(containerName.mid(1,2).toInt(), containerName.mid(3,2).toInt(), containerName.mid(0,1).toStdString());
    }
    if(type == "chosen"){ //container to be moved
        if(containerName[0] == 't'){
            label->setStyleSheet("border:3px solid #51E260;");
        } else {

            label->setStyleSheet("background-color: #51E260;" + border);
        }
    } else if (type == "goal"){ //location for container to be moved to
        if(containerName[0] == 't'){
            label->setStyleSheet("border:3px solid #F7F02B;");
        } else {
            label->setStyleSheet("background-color: #F7F02B;");
        }
        connect(flashTimer, SIGNAL(timeout()), this, SLOT(toggleLabelVisibility()));
        flashTimer->start(750);
    } else if (type == "NAN"){
        label->setStyleSheet("background-color: #C9C9C9;" + border);
    } else if (type == "set"){ //set location as container
        if(containerName[0] == 't'){
            label->setVisible(false);
        } else {
            label->setStyleSheet("background-color: #498E93;" + border);
        }
    } else { //location now empty
        if(containerName[0] == 't'){
            label->setVisible(false);
        } else {
            label->setStyleSheet("background-color: white;" + border);
        }
    }
}

void MainWindow::initial_container_setup(){
    vector<Container*> containers = CurrentOperation->get_NAN_containers();
    for(int i =0; i < containers.size(); i++){
        string loc = containers[i]->get_location();
        string loc_format = loc[0] + loc.substr(2,2) + loc.substr(5,2);
        QString location = QString::fromStdString(loc_format);
        set_container_style(location, "NAN");
    }
    containers = CurrentOperation->get_containers();
    for(int i =0; i < containers.size(); i++){
        string loc = containers[i]->get_location();
        string loc_format = loc[0] + loc.substr(2,2) + loc.substr(5,2);
        QString location = QString::fromStdString(loc_format);
        set_container_style(location, "set");
    }
}

QString MainWindow::get_border_type(int row, int col, string loc){\
    if(loc == "s"){
        if (row == 8 && col == 12){
            return "border: 0.5px solid black;";
        } else if (col == 12) {
            return "border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-right: 0.5px solid black;";
        } else  if (row == 8){
            return "border-top: 0.5px solid black; border-left: 0.5px solid black; border-bottom:0.5px solid black;";
        } else {
            return "border-left: 0.5px solid black; border-bottom: 0.5px solid black;";
        }
    } else if (loc == "b"){
        if( row == 4 && col == 24){
            return "border: 0.5px solid black;";
        }else if (col == 24) {
            return "border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-right: 0.5px solid black;";
        } else  if (row == 4){
            return "border-top: 0.5px solid black; border-left: 0.5px solid black; border-bottom:0.5px solid black;";
        } else {
            return "border-left: 0.5px solid black; border-bottom: 0.5px solid black;";
        }
    }

}

void MainWindow::update_container_styles(){
    cout << "update container styles after move" << endl;
    QString chosen = CurrentOperation->get_current_container();
    set_container_style(chosen, "");
    QString goal = CurrentOperation->get_goal_loc();
    set_container_style(goal, "set");

}
