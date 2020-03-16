#ifndef SP3_H_
#define SP3_H_

#include <cstdint>
#include <string>
#include <vector>

namespace parsers
{

enum class SP3Version
{
    UNKNOWN,
    VERSION_C
};

enum class SP3PosVelFlag
{
    UNKNOWN,
    POSITION,
    VELOCITY
};


class SP3
{
  public:
    SP3(const std::string& filename);

    bool valid();

  private:
    bool handle_line0(const std::string& line);
    bool handle_line1(const std::string& line);
    bool handle_line2(const std::string& line);
    bool handle_lines3_6(const std::string& line);
    bool handle_lines7_12(const std::string& line);

    bool valid_;

    SP3Version version_;
    SP3PosVelFlag pos_vel_flag_;

    uint16_t start_year_;
    uint8_t start_month_;
    uint8_t start_day_;

    uint8_t start_hour_;
    uint8_t start_minute_;
    float start_second_;

    uint16_t num_epochs_;
    uint16_t data_used_;

    std::string coordinate_system_;
    std::string orbit_type_;
    std::string agency_;

    // Line 2.
    uint16_t gps_week_;
    double seconds_of_week_;
    double epoch_interval_;
    uint16_t mod_jul_day_st_;
    double fractional_day_;

    // Line 3.
    uint16_t num_sats_;
    std::vector<std::string> sats_;
    std::vector<uint8_t> sats_accuracy_;

};

}  // namespace parsers

#endif
