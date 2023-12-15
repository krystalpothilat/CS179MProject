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
    ui->NoteInput->setPlaceholderText("Enter Note Here");
    ui->UserNameInput->setVisible(false);
    ui->UserNameInput->setPlaceholderText("Enter Name Here");

    ui->stackedWidget->setCurrentIndex(0);
    QListWidgetItem *item1 = new QListWidgetItem();
    QListWidgetItem *item2 = new QListWidgetItem();
    Manifest = item1;
    UserName = item2;
    set_up_animation();
    currentUTCtime = QDateTime::currentDateTimeUtc();
    pacificTimeZone = QTimeZone("America/Los_Angeles");
    flashTimer = new QTimer(this);

    //get/set log path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    cout << "appDataPath: " << appDataPath.toStdString() << endl;
    //check directory exists
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName = "Log2024.txt";
    QString filePath = appDataPath + QDir::separator() + fileName;
    cout << "logpath = " << filePath.toStdString() << endl;
    if (QFile::exists(filePath)) {
        qlogpath = filePath;
    } else {
        // create new log with fileName
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
            qDebug() << "Empty file created.";
        } else {
            qDebug() << "Error creating a new file.";
        }
    }

}

MainWindow::~MainWindow()
{
    ui->LoadContainerDisplay->clear();
    ui->unLoadContainerDisplay->clear();
    ui->UserNameDisplay->clear();
    ui->ManifestDisplay->clear();
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
    if(to_be_completed_moves.empty()){
        ui->stackedWidget->setCurrentIndex(5);
        QCoreApplication::processEvents();
        save();
        showDialog("No moves need to be made.");
        return;
    }
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
    sort(to_be_unloaded_options.begin(), to_be_unloaded_options.end(),
         [](Container* a, Container* b) {
             return a->get_description() < b->get_description();
         });
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
    string chosen_coord;
    string chosen_coord_format;
    string chosen_coordname;
    string goal_coord;
    string goal_coord_format;
    string goal_coordname;
    int manifest_line_chosen;
    int manifest_line_goal;

    if (currentContainer->get_location() == "t") {
        location = "Truck";
        chosen_coordname = "truckborder";
    } else if (currentContainer->get_location().at(0) == 'b') {
        chosen_coord = currentContainer->get_location().substr(2, 5);
        chosen_coord_format = chosen_coord.substr(0,2) + chosen_coord.substr(3,2);
        location = "Buffer at " + chosen_coord;
        chosen_coordname = "b" + chosen_coord_format;
    } else {
        chosen_coord = currentContainer->get_location().substr(2, 5);
        chosen_coord_format = chosen_coord.substr(0,2) + chosen_coord.substr(3,2);
        location = "Ship at " + chosen_coord;
        chosen_coordname = "s" + chosen_coord_format;
    }

    set_container_style(QString::fromStdString(chosen_coordname), "chosen");
    CurrentOperation->set_current_container(QString::fromStdString(chosen_coordname));

    if(chosen_coordname != "truckborder"){
        manifest_line_chosen = to_line_index(chosen_coord_format);
    }

    if (to_be_completed_moves.at(i)->get_final_location() == "t") {
        finalLocation = "Truck";
        goal_coordname = "truckborder";
    } else if (to_be_completed_moves.at(i)->get_final_location().at(0) == 'b') {
        goal_coord = to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        goal_coord_format = goal_coord.substr(0,2) + goal_coord.substr(3,2);
        finalLocation = "Buffer at " + goal_coord;
        goal_coordname = "b" + goal_coord_format;
    } else {
        goal_coord = to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        goal_coord_format = goal_coord.substr(0,2) + goal_coord.substr(3,2);
        finalLocation = "Ship at " + to_be_completed_moves.at(i)->get_final_location().substr(2, 5);
        goal_coordname = "s" + goal_coord_format;
    }

    set_container_style(QString::fromStdString(goal_coordname), "goal");
    CurrentOperation->set_goal_loc(QString::fromStdString(goal_coordname));

    if(goal_coordname != "truckborder"){
        manifest_line_goal = to_line_index(goal_coord_format);
    }

    moveoutput = "Step " + to_string(i + 1) + " of " + to_string(to_be_completed_moves.size()) + "\n";
    moveoutput += "Move " + currentContainer->get_description() + " from " + location + " to " + finalLocation + "\n";
    moveoutput += "Time for this move: " + to_string(to_be_completed_moves.at(i)->get_time()) + " Minutes\n";
    moveoutput += "Time remaining: " + to_string(time) + " Minutes\n";
    ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
    QString userInputWeight = "";
    if (currentContainer->get_weight() == -1) {
        ui->weightinput->setVisible(true);
        ui->weightprompt->setVisible(true);
        userInputWeight = ui->weightinput->text();
    } else {
        userInputWeight = QString::number(currentContainer->get_weight());
        moveoutput += "Weight: " + userInputWeight.toStdString() + " Kilograms";
        ui->stepxofxdisplay->setText(QString::fromStdString(moveoutput));
        ui->Step_X_of_X_Confirm->setVisible(true);
    }

    //format log message for move update
    moveLogMessage = "Container " + currentContainer->get_description() + ", weighing " + userInputWeight.toStdString() + " kilos has been moved from ";
    if(location.substr(0,4) == "Ship"){ //container moved from location on ship
        update_manifest(manifest_line_chosen, "UNUSED", 0);
        moveLogMessage += "[" + chosen_coord + "] on Ship to ";
    } else if (location == "Truck") { //moved from truck
        moveLogMessage += "truck to ";
    } else { //moved from buffer
        moveLogMessage += "[" + chosen_coord + "] on buffer to ";
    }

    if(finalLocation.substr(0,4) == "Ship"){ //container moved to location on ship
        update_manifest(manifest_line_goal, currentContainer->get_description(), userInputWeight.toInt());
        moveLogMessage += "[" + goal_coord + "] on Ship";
    } else if (finalLocation == "Truck"){ //moved to truck
        moveLogMessage += "truck";
    } else { //moved to buffer
        moveLogMessage += "[" + goal_coord + "] on buffer";
    }
}

//Add leading zeros to container measurements
string MainWindow::appendLeadingZeros(string input, int width) {
    ostringstream oss;
    oss << setw(width) << setfill('0') << input;
    return oss.str();
}

//Get index of line in manifest
int MainWindow::to_line_index(string s){
    int row = std::stoi(s.substr(0,2));
    int col = std::stoi(s.substr(2,2));
    return ( ((row-1) * 12) + col - 1);
}

//Get coordinates from index of line in manifest
string MainWindow::to_coordinates(int i){
    int row = ((i+1)/12) + 1;
    int col = (i+1) % 12;
    return appendLeadingZeros(to_string(row), 2) + "," + appendLeadingZeros(to_string(col), 2);
}

void MainWindow::save()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if(!desktopPath.isEmpty()){
        QString newFilePath = desktopPath  + QString::fromStdString("/" + filename + "_OUTBOUND.txt");
        QFile file(newFilePath);

        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            QStringList list = CurrentOperation->get_manifestlines();
            for(const QString& line : list){
                out << line << "\n";
            }

            file.close();
            filepath = newFilePath.toStdString();
            string output = "Manifest: " + filename + "_OUTBOUND.txt has been saved to:\n " + filepath;
            showDialog("Please send outbound manifest to captain.");
            ui->downloadmanifestdisplay->setText(QString::fromStdString(output));
            ui->Download_Manifest_Confirm->setVisible(true);
            string logmessage = "Finished a cycle. Manifest " + filename + "_OUTBOUND.txt was written to " + filepath + ", and a reminder pop-up to send the file was displayed";
            updatelog(logmessage);
        } else {
            qDebug() << "Failed to open the file for writing:" << file.errorString();
        }
    } else {
        qDebug() << "Failed to get the desktop path.";
    }

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

//Log:
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

//Manifest Updates:
void MainWindow::update_manifest(int i, string desc, int weight){
    cout << "update_manifest" << endl;
    string coord = to_coordinates(i);
    string line = "[" + coord + "], {" + appendLeadingZeros(to_string(weight), 5) + "}, " + desc;
    cout << "new manifest line : " << line << endl;
    CurrentOperation->set_manifest_line(i, QString::fromStdString(line));
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
        QCoreApplication::processEvents();
        get_unload_options();
        string description = "Manifest " + filename + " is opened, there are " + to_string(to_be_unloaded_options.size()) + " containers on the ship";
        updatelog(description);
        set_up_animation();
        initial_container_setup();
    } else {
        ui->stackedWidget->setCurrentIndex(3);
        QCoreApplication::processEvents();
        string description = "Manifest " + filename + " is opened, there are " + to_string(to_be_unloaded_options.size()) + " containers on the ship";
        updatelog(description);
        set_up_animation();
        initial_container_setup();
        calculate();
    }
}

void MainWindow::on_UploadManifestSelectFile_clicked()
{
    QString qfilepath = QFileDialog::getOpenFileName(this, "Open Manifest",QDir::homePath(),"Text Files (*.txt)");
    string tempfilepath = qfilepath.toStdString();
    ui->Upload_Manifest_Confirm->setVisible(false);
    if (tempfilepath!= "") {
        filepath = qfilepath.toStdString();
        QFileInfo fileInfo(qfilepath);
        QString qfilename = fileInfo.fileName();
        cout << qfilename.toStdString() << endl;
        QString qfilenameWithoutTxt = removeTxtExtension(qfilename);
        filename = qfilenameWithoutTxt.toStdString();
        Manifest->setText(qfilenameWithoutTxt);
        if(ui->ManifestDisplay->count()==0){
            ui->ManifestDisplay->addItem(Manifest);
        }
        ui->Upload_Manifest_Confirm->setVisible(true);
    }
    else{
        if(ui->ManifestDisplay->count()!=0){
            Manifest->setText("");
        }
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
    string container = qcontainer.toStdString();
    if(container.length()>MAXCHARLIMIT){
        ui->UserNameInput->setText("");
        string charactersOverLimit = to_string(container.length()-MAXCHARLIMIT);
        string output = "Container Description cannot be greater than " +to_string(MAXCHARLIMIT)+ " Characters.\nYou are over the character limit by:\n"+charactersOverLimit+" characters.";
        ui->LoadContainerInput->setText("");
        showDialog(QString::fromStdString(output));
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
    to_be_loaded.push_back(new Container("t", container, -1));
    ui->LoadContainerInput->setText("");
}

void MainWindow::on_Select_Load_Confirm_clicked()
{
    CurrentOperation->set_load(to_be_loaded);
    ui->stackedWidget->setCurrentIndex(3);
    QCoreApplication::processEvents();
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
    updatelog(moveLogMessage);
    index++;
    if (index == to_be_completed_moves.size()) {
        ui->stackedWidget->setCurrentIndex(5);
        QCoreApplication::processEvents();
        save();
    } else {
        display_move(index);
    }
}

//Download Manifest Slots:
void MainWindow::on_Download_Manifest_Confirm_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    Manifest->setText("");
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
    if(name.length()>MAXCHARLIMIT){
        ui->UserNameInput->setText("");
        string charactersOverLimit = to_string(name.length()-MAXCHARLIMIT);
        string output = "Name cannot be greater than " +to_string(MAXCHARLIMIT)+ " Characters.\nYou are over the character limit by:\n"+charactersOverLimit+" characters.";
        showDialog(QString::fromStdString(output));
        return;
    }
    UserName->setText(qname);
    if(ui->UserNameDisplay->count()==0){
        ui->UserNameDisplay->addItem(UserName);
    }
    CurrentOperation->set_username(name);
    ui->UserNameInput->setText("");
    ui->UserNameInput->setVisible(false);
    updatelog(name + " signs in");
}

void MainWindow::on_NoteInput_returnPressed()
{
    QString qnote = ui->NoteInput->text().trimmed();
    string note = qnote.toStdString();
    if (containsNonPrintableCharacters(qnote)||qnote.isEmpty()) {
        ui->NoteInput->setText("");
        showDialog("Please enter at least one printable character.");
        return;
    }
    if(note.length()>MAXCHARLIMIT){
        int numChunks = (note.length() + MAXCHARLIMIT - 1) / MAXCHARLIMIT;

        for (int i = 0; i < numChunks; ++i) {
            int startIdx = i * MAXCHARLIMIT;
            int endIdx = startIdx + MAXCHARLIMIT;
            if (endIdx > note.length()) {
                endIdx = note.length();
            }

            string chunk = note.substr(startIdx, endIdx - startIdx);
            CurrentOperation->set_note(chunk);
        }
        ui->NoteInput->setText("");
        ui->NoteInput->setVisible(false);
        return;
    }
    CurrentOperation->set_note(note);
    ui->NoteInput->setText("");
    ui->NoteInput->setVisible(false);
    updatelog("NOTE: " + note);
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
    QString style = "";
    if(containerName[0] != 't'){
        style = get_style(containerName.mid(1,2).toInt(), containerName.mid(3,2).toInt(), containerName.mid(0,1).toStdString(), type);
    }
    if(type == "chosen"){ //container to be moved
        cout << "setting chosen" << endl;
        if(containerName[0] == 't'){
            label->setStyleSheet("border:3px solid #51E260;");
        } else {
            label->setStyleSheet(style);
        }
    } else if (type == "goal"){ //location for container to be moved to
        cout << "setting goal" << endl;
        if(containerName[0] == 't'){
            label->setStyleSheet("border:3px solid #F7F02B;");
        } else {
            label->setStyleSheet(style);
        }
        connect(flashTimer, SIGNAL(timeout()), this, SLOT(toggleLabelVisibility()));
        flashTimer->start(750);
    } else if (type == "NAN"){
        cout << "setting NAN" << endl;
        label->setStyleSheet(style);
    } else if (type == "set"){ //set location as container
        cout << "setting container" << endl;
        if(containerName[0] == 't'){
            label->setVisible(false);
        } else {
            label->setStyleSheet(style);
        }
    } else { //location now empty
        cout << "setting to empty" << endl;
        if(containerName[0] == 't'){
            label->setVisible(false);
        } else {
            label->setStyleSheet(style);
        }
    }
}

void MainWindow::initial_container_setup(){
    cout << "initial container setup" << endl;
    vector<Container*> containers = CurrentOperation->get_NAN_containers();
    for(size_t i =0; i < containers.size(); i++){
        string loc = containers[i]->get_location();
        string loc_format = loc[0] + loc.substr(2,2) + loc.substr(5,2);
        QString location = QString::fromStdString(loc_format);
        set_container_style(location, "NAN");
    }
    containers = CurrentOperation->get_containers();
    for(size_t i=0; i < containers.size(); i++){
        string loc = containers[i]->get_location();
        string loc_format = loc[0] + loc.substr(2,2) + loc.substr(5,2);
        QString location = QString::fromStdString(loc_format);
        set_container_style(location, "set");
    }
}

QString MainWindow::get_style(int row, int col, string loc, string type){
    QString style;
    if(type == "chosen"){ //container to be moved
        style = "background-color: #51E260;";
    } else if (type == "goal"){ //location for container to be moved to
        style ="background-color: #F7F02B;";
    } else if (type == "NAN"){
        style = "background-color: #C9C9C9;";
    } else if (type == "set"){ //set location as container
        style = "background-color: #498E93;";
    } else { //location now empty
        style = "background-color: white;";
    }

    if(loc == "s"){
        if (row == 8 && col == 12){
            cout << "ship border top right" << endl;
            style += "border: 0.5px solid black;";
        } else if (col == 12) {
            cout << "ship right border" << endl;
            style += "border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-right: 0.5px solid black;";
        } else  if (row == 8){
            cout << "ship top row border" << endl;
            style += "border-top: 0.5px solid black; border-left: 0.5px solid black; border-bottom:0.5px solid black;";
        } else {
            cout << "ship any other" << endl;
            style += "border-left: 0.5px solid black; border-bottom: 0.5px solid black;";
        }
    } else if (loc == "b"){
        if( row == 4 && col == 24){
            cout << "buffer border top right" << endl;
            style += "border: 0.5px solid black;";
        }else if (col == 24) {
            cout << "buffer right border" << endl;
            style += "border-left: 0.5px solid black; border-bottom: 0.5px solid black; border-right: 0.5px solid black;";
        } else  if (row == 4){
            cout << "buffer top row border" << endl;
            style += "border-top: 0.5px solid black; border-left: 0.5px solid black; border-bottom:0.5px solid black;";
        } else {
            cout << "buffer any other" << endl;
            style += "border-left: 0.5px solid black; border-bottom: 0.5px solid black;";
        }
    }

    return style;

}

void MainWindow::update_container_styles(){
    cout << "update container styles after move" << endl;
    QString chosen = CurrentOperation->get_current_container();
    set_container_style(chosen, "");
    QString goal = CurrentOperation->get_goal_loc();
    set_container_style(goal, "set");

}
