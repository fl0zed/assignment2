#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "MQTTClient.h"

// --- MQTT Configuration Constants ---
#define ADDRESS     "tcp://127.0.0.1:1883"  // The broker IP (localhost in this case)
#define CLIENTID    "rpi5"                  // Unique identifier for this MQTT client
#define AUTHMETHOD  "molloyd"               // Username for broker authentication
#define AUTHTOKEN   "password"               // Password/Token for broker authentication
#define TOPIC       "een1071/CPUTemp"       // The specific topic to publish/subscribe to
#define PAYLOAD     "Hello World!"          // Default message content
#define QOS         1                       // Quality of Service: 1 = At least once delivery
#define TIMEOUT     10000L                  // Connection timeout in milliseconds

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow; // Forward declaration of the UI class generated from mainwindow.ui
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The main interface class handling UI logic and MQTT communication.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // UI Interaction Slots (Auto-connected via name matching)
    void on_downButton_clicked();
    void on_upButton_clicked();
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();

    // Custom slot to handle incoming MQTT messages within the Qt main thread
    void on_MQTTmessage(QString message);

signals:
    // Signal used to pass data from the MQTT callback thread to the UI thread
    void messageSignal(QString message);

private:
    Ui::MainWindow *ui;         // Pointer to the visual elements of the window
    void update();              // Internal function to refresh UI state
    int count, time;            // Variables for tracking application state/telemetry

    MQTTClient client;          // The Paho MQTT client instance
    volatile MQTTClient_deliveryToken deliveredtoken; // Token used to track message delivery

    // Friend declarations allow external C-style callback functions to access
    // private members of this class (essential for Paho MQTT integration).
    friend void delivered(void *context, MQTTClient_deliveryToken dt);
    friend int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    friend void connlost(void *context, char *cause);
};

// --- External C-style Callback Prototypes ---
// These functions are required by the Paho MQTT library to handle asynchronous events.
void delivered(void *context, MQTTClient_deliveryToken dt);
int  msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);

#endif // MAINWINDOW_H
