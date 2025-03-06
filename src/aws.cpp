#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mqtt/client.h>

#define ADDRESS     "ssl://abr0s9fb0umjr-ats.iot.us-east-2.amazonaws.com:8883"
#define CLIENTID    "iotconsole-e06a5ad6-d040-4427-ac3e-55a489856c46"
#define TOPIC       "road/damage"
#define QOS         1
#define TIMEOUT     std::chrono::seconds(10)

// AWS IoT Core Credentials
#define CA_PATH     "/home/ubuntu/aws_gp/root-ca.pem"
#define CERT_PATH   "/home/ubuntu/aws_gp/certificate.pem.crt"
#define KEY_PATH    "/home/ubuntu/aws_gp/private.pem.key"

// Function to read text file and check for "detected"
bool checkForDetection(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("detected") != std::string::npos) {
            return true; // Found "detected" in the file
        }
    }
    return false;
}

int main() {
    try {
        // Create MQTT client
        mqtt::client client(ADDRESS, CLIENTID);

        // Configure SSL options
        mqtt::ssl_options ssl_opts;
        ssl_opts.set_trust_store(CA_PATH);
        ssl_opts.set_key_store(CERT_PATH);
        ssl_opts.set_private_key(KEY_PATH);

        // Configure connection options
        mqtt::connect_options conn_opts;
        conn_opts.set_ssl(ssl_opts);
        conn_opts.set_keep_alive_interval(std::chrono::seconds(20));
        conn_opts.set_clean_session(true);

        // Connect to AWS IoT Core
        std::cout << "Connecting to AWS IoT Core..." << std::endl;
        client.connect(conn_opts);
        std::cout << "Connected to AWS IoT Core!" << std::endl;

        while (true) {
            if (checkForDetection("/home/ubuntu/aws_gp/data.txt")) {
                std::cout << "Detected! Sending alert to AWS IoT Core..." << std::endl;

                // mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, " detected", QOS, false);
                std::string payload = R"({"status": "road damage detected"})";
mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, payload, QOS, false);
                client.publish(pubmsg);

                std::cout << "Message sent!" << std::endl;
            } else {
                std::cout << "No detection found." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        // Disconnect
        client.disconnect();
        std::cout << "Disconnected from AWS IoT Core." << std::endl;

    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT Exception: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
