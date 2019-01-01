#include <Common/Ublox.h>
#include <Common/Util.h>

using namespace std;

class GPS
{
  private:
    // create ublox class instance
    Ublox gps;
    bool ready = false;
    double *status;

  public:
    void initialize(double *status)
    {
        this->status = status;
        if (gps.testConnection())
        {
            printf("Ublox test OK\n");
            if (!gps.configureSolutionRate(1000))
            {
                printf("Setting new rate: FAILED\n");
            }
            else
            {
                ready = true;
            }
        }
    }

    void update()
    {
        std::vector<double> pos_data;

        // Here we test connection with the receiver. Function testConnection() waits for a ubx protocol message and checks it.
        // If there's at least one correct message in the first 300 symbols the test is passed

        // gps.decodeMessages();
        // You can use this function to decode all messages, incoming from the GPS receiver. The function starts an infinite loop.
        // In this example we can only decode NAV_STATUS and NAV-POSLLH messages, the others are simply ignored.
        // You can add new message types in function decodeMessage() of class UBXParser(see ublox.h)

        // Here, however we use a different approach. Instead of trying to extract info from every message(as done in decodeMessages()),
        // this function waits for a message of a specified type and gets you just the information you need
        // In this example we decode NAV_STATUS and NAV-POSLLH messages, adding new types, however is quite easy

        if (gps.decodeSingleMessage(Ublox::NAV_POSLLH, pos_data) == 1)
        {
            // after desired message is successfully decoded, we can use the information stored in pos_data vector
            // right here, or we can do something with it from inside decodeSingleMessage() function(see ublox.h).
            // the way, data is stored in pos_data vector is specified in decodeMessage() function of class UBXParser(see ublox.h)
            status[status_gps_longitude] = pos_data[1] / 10000000;
            status[status_gps_latitude] = pos_data[2] / 10000000;
            status[status_gps_height] = pos_data[4] / 1000; // sea level
            status[status_gps_v_accuracy] = pos_data[5] / 1000;
            status[status_gps_h_accuracy] = pos_data[6] / 1000;
        }
        else
        {
            // printf("Message not captured\n");
            // use this to see, how often you get the right messages
            // to increase the frequency you can turn off the undesired messages or tweak ublox settings
            // to increase internal receiver frequency
        }

        if (gps.decodeSingleMessage(Ublox::NAV_STATUS, pos_data) == 1)
        {
            status[status_gps_fix] = pos_data[0]; // Fix status
            std::cout << "[ GPS ] : status gps fix : " << status[status_gps_fix] << "\n";
            /*
            */
            status[status_gps_get_fix] = pos_data[1]; // getFixOk
            
            /*printf("gpsFixOk: %d\n", ((int)pos_data[1] & 0x01));

            printf("gps Fix status: ");
            switch ((int)pos_data[0])
            {
            case 0x00:
                printf("no fix\n");
                break;

            case 0x01:
                printf("dead reckoning only\n");
                break;

            case 0x02:
                printf("2D-fix\n");
                break;

            case 0x03:
                printf("3D-fix\n");
                break;

            case 0x04:
                printf("GPS + dead reckoning combined\n");
                break;

            case 0x05:
                printf("Time only fix\n");
                break;

            default:
                printf("Reserved value. Current state unknown\n");
                break;
            }*/
        }
    }
};