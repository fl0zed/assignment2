#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

// Global pointer to the MainWindow instance.
// This allows C-style callback functions (which don't know about 'this')
// to communicate back to our Qt object.
MainWindow *handle;

/**
 * @brief Constructor for MainWindow.
 * Initializes the UI, sets up the real-time graph, and connects internal signals.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->count = 50;  // Initial data value
    this->time = 0;
    this->setWindowTitle("EEN1071 Assignment 2");

    // --- QCustomPlot Setup ---
    this->ui->customPlot->addGraph();
    this->ui->customPlot->yAxis->setLabel("Pitch (degrees)");

    // Configure X-axis to show a rolling time scale
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->ui->customPlot->xAxis->setTicker(timeTicker);
    this->ui->customPlot->yAxis->setRange(-180, 180); // Set fixed scale for pitch
    this->ui->customPlot->replot();

    // Internal Signal/Slot connection to handle thread-safe UI updates from MQTT
    QObject::connect(this, SIGNAL(messageSignal(QString)),
                     this, SLOT(on_MQTTmessage(QString)));

    // Assign this instance to the global handle for callback access
    ::handle = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Updates the real-time plot and the status text field.
 */
void MainWindow::update(){
    static QTime startTime = QTime::currentTime();
    double key = startTime.msecsTo(QTime::currentTime()) / 1000.0; // Calculate elapsed seconds

    // Add new data point to the graph
    ui->customPlot->graph(0)->addData(key, count);
    ui->customPlot->graph(0)->rescaleKeyAxis(true); // Automatically scroll the X-axis
    ui->customPlot->replot();

    QString text = QString("Value added is %1").arg(this->count);
    ui->outputEdit->setText(text);
}

// Manual increment/decrement of the data value for testing
void MainWindow::on_downButton_clicked() { this->count -= 10; this->update(); }
void MainWindow::on_upButton_clicked()   { this->count += 10; this->update(); }

/**
 * @brief Initializes and connects the Paho MQTT client to the broker.
 */
void MainWindow::on_connectButton_clicked()
{
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;

    // Create the client instance
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    // Register the C-style callback functions
    if (MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered) == 0){
        ui->outputText->appendPlainText(QString("Callbacks set correctly"));
    }

    // Attempt to connect to the broker defined in mainwindow.h
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        ui->outputText->appendPlainText(QString("Failed to connect, return code %1").arg(rc));
    }

    // Subscribe to the telemetry topic
    ui->outputText->appendPlainText(QString("Subscribing to topic " TOPIC " for client " CLIENTID));
    int x = MQTTClient_subscribe(client, TOPIC, QOS);
    ui->outputText->appendPlainText(QString("Result of subscribe is %1 (0=success)").arg(x));
}

// --- MQTT CALLBACK FUNCTIONS (C-Style) ---

void delivered(void *context, MQTTClient_deliveryToken dt) {
    (void)context;
    qDebug() << "Message delivery confirmed";
    handle->deliveredtoken = dt;
}

/**
 * @brief Triggered when a message arrives from the broker.
 * RUNS IN A SEPARATE THREAD. Emits a signal to move the data to the UI thread.
 */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    (void)context; (void)topicLen;

    // Convert the raw payload bytes into a QString
    QString payload = QString::fromUtf8((const char*)message->payload, message->payloadlen);

    // Thread-safe: emit signal to MainWindow rather than calling UI methods directly
    emit handle->messageSignal(payload);

    // Clean up memory allocated by the Paho library
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/**
 * @brief Slot triggered by messageSignal.
 * Safely updates the UI with the MQTT payload from the main thread.
 */
void MainWindow::on_MQTTmessage(QString payload){
    ui->outputText->appendPlainText(payload);
    ui->outputText->ensureCursorVisible();
}

void connlost(void *context, char *cause) {
    (void)context; (void)*cause;
    qDebug() << "Connection Lost" << Qt::endl;
}

/**
 * @brief Gracefully disconnects from the MQTT broker.
 */
void MainWindow::on_disconnectButton_clicked()
{
    qDebug() << "Disconnecting from the broker" << Qt::endl;
    MQTTClient_disconnect(client, 10000); // 10 second timeout for quiescing
}
