// ftp://igs.org/pub/data/format/sp3c.txt

#include <parsers/sp3.h>
#include <spdlog/spdlog.h>

#include <fstream>

namespace parsers
{

SP3::SP3(const std::string& filename) :
    valid_{false},
    version_{SP3Version::UNKNOWN},
    pos_vel_flag_{SP3PosVelFlag::UNKNOWN},
    start_year_{0u},
    start_month_{0u},
    start_day_{0u},
    start_hour_{0u},
    start_minute_{0u},
    start_second_{0.0f},
    num_epochs_{0u},
    data_used_{0u},
    coordinate_system_{},
    orbit_type_{},
    agency_{},
    gps_week_{0u},
    seconds_of_week_{0.0},
    epoch_interval_{0.0},
    mod_jul_day_st_{0u},
    fractional_day_{0.0},
    num_sats_{0u},
    sats_{},
    sats_accuracy_{}
{
    std::ifstream input = std::ifstream(filename, std::ios::in);
    if (input.is_open() == false)
    {
        valid_ = false;
        return;
    }

    valid_ = true;

    std::string line;
    for (size_t line_num = 0; std::getline(input, line); ++line_num)
    {
        switch (line_num)
        {
            case (0):
                valid_ &= handle_line0(line);
                break;

            case (1):
                valid_ &= handle_line1(line);
                break;

            case (2):
                valid_ &= handle_line2(line);
                break;

            case (3):
            case (4):
            case (5):
            case (6):
                valid_ &= handle_lines3_6(line);
                break;

            case (7):
            case (8):
            case (9):
            case (10):
            case (11):
            case (12):
                valid_ &= handle_lines7_12(line);
                break;

            default:
                break;
        }
    }
}

bool SP3::handle_line0(const std::string& line)
{
    // SP3 First Line

    // Columns             Description        Example             Format
    // -----------------   -----------------  -----------------   ---------
    // Columns 1-2         Version Symbol     #c                  A2
    // Column  3           Pos or Vel Flag    P or V              A1
    // Columns 4-7         Year Start         2001                I4
    // Column  8           Unused             _                   blank
    // Columns 9-10        Month Start        _8                  I2
    // Column  11          Unused             _                   blank
    // Columns 12-13       Day of Month St    _8                  I2
    // Column  14          Unused             _                   blank
    // Columns 15-16       Hour Start         _0                  I2
    // Column  17          Unused             _                   blank
    // Columns 18-19       Minute Start       _0                  I2
    // Column  20          Unused             _                   blank
    // Columns 21-31       Second Start       _0.00000000         F11.8
    // Column  32          Unused             _                   blank
    // Columns 33-39       Number of Epochs   ____192             I7
    // Column  40          Unused             _                   blank
    // Columns 41-45       Data Used          ____d               A5
    // Column  46          Unused             _                   blank
    // Columns 47-51       Coordinate Sys     ITR97               A5
    // Column  52          Unused             _                   blank
    // Columns 53-55       Orbit Type         FIT                 A3
    // Column  56          Unused             _                   blank
    // Columns 57-60       Agency             _NGS                A4

    if (line.size() != 60)
    {
        SPDLOG_ERROR("Invalid line length of {}, expected 60.", line.size());
        return false;
    }

    // Columns 1-2 Version Symbol.
    if ((line[0] == '#') && (line[1] == 'c'))
    {
        version_ = SP3Version::VERSION_C;
    }
    else
    {
        SPDLOG_ERROR("Unknown version `{}`.", line[1]);
        return false;
    }

    // Column  3 Pos or Vel Flag.
    if (line[2] == 'P')
    {
        pos_vel_flag_ = SP3PosVelFlag::POSITION;
    }
    else if (line[2] == 'V')
    {
        pos_vel_flag_ = SP3PosVelFlag::VELOCITY;
    }
    else
    {
        SPDLOG_ERROR("Unknown PosVelFlag `{}`.", line[2]);
        return false;
    }

    // Columns 4-7 Year Start.
    start_year_ = std::atoi(&line[3]);

    // Columns 9-10 Month Start.
    start_month_ = std::atoi(&line[8]);

    // Columns 12-13 Day of Month St.
    start_day_ = std::atoi(&line[11]);

    // Columns 15-16 Hour Start.
    start_hour_ = std::atoi(&line[14]);

    // Columns 18-19 Minute Start.
    start_minute_ = std::atoi(&line[17]);

    // Columns 21-31 Second Start.
    start_second_ = std::atof(&line[20]);

    SPDLOG_DEBUG("Started {:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02.3f}", start_year_, start_month_, start_day_,
        start_hour_, start_minute_, start_second_);

    // Columns 33-39 Number Of Epochs.
    num_epochs_ = std::atoi(&line[32]);

    // Columns 41-45 Data Used.
    data_used_ = std::atoi(&line[40]);

    SPDLOG_DEBUG("Num Epochs: {}, Data Used: {}", num_epochs_, data_used_);

    // Columns 47-51 Coordinate Sys.
    coordinate_system_ = std::string(&line[46], 5);

    // Columns 53-55 Orbit Type.
    orbit_type_ = std::string(&line[52], 3);

    // Columns 57-60 Agency.
    agency_ = std::string(&line[56], 4);

    SPDLOG_DEBUG("Coordinate System: {}, Orbit Type: {}, Agency: {}", coordinate_system_, orbit_type_, agency_);

    return true;
}

bool SP3::handle_line1(const std::string& line)
{

    // SP3 Line Two

    // Columns             Description        Example             Format
    // -----------------   -----------------  -----------------   ---------
    // Columns 1-2         Symbols            ##                  A2
    // Column  3           Unused             _                   blank
    // Columns 4-7         GPS Week           1126                I4
    // Column  8           Unused             _                   blank
    // Columns 9-23        Seconds of Week    259200.00000000     F15.8
    // Column  24          Unused             _                   blank
    // Columns 25-38       Epoch Interval     __900.00000000      F14.8
    // Column  39          Unused             _                   blank
    // Columns 40-44       Mod Jul Day St     52129               I5
    // Column  45          Unused             _                   blank
    // Columns 46-60       Fractional Day     0.0000000000000     F15.13

    if (line.size() != 60)
    {
        SPDLOG_ERROR("Invalid line length of {}, expected 60.", line.size());
        return false;
    }

    gps_week_ = std::atoi(&line[3]);
    seconds_of_week_ = std::atof(&line[8]);
    epoch_interval_ = std::atof(&line[24]);
    mod_jul_day_st_ = std::atoi(&line[39]);
    fractional_day_ = std::atof(&line[45]);

    SPDLOG_DEBUG("GPS Week: {}, Seconds Of Week: {}, Epoch Interval: {}, Mod Jul: {}, Fractional Day: {}",
        gps_week_, seconds_of_week_, epoch_interval_, mod_jul_day_st_, fractional_day_);

    return true;
}

bool SP3::handle_line2(const std::string& line)
{
    // SP3 Line Three

    // Columns             Description        Example             Format
    // -----------------   -----------------  -----------------   ---------
    // Columns 1-2         Symbols            +_                  A2
    // Column  3-4         Unused             __                  2 blanks
    // Columns 5-6         Number of Sats     26                  I2
    // Column  7-9         Unused             ___                 3 blanks
    // Columns 10-12       Sat #1 Id          G01                 A1,I2.2
    // Column  13-15       Sat #2 Id          G02                 A1,I2.2
    //        *
    //        *
    //        *
    // Columns 58-60       Sat #17 Id         G21                 A1,I2.2

    if (line.size() != 60)
    {
        SPDLOG_ERROR("Invalid line length of {}, expected 60.", line.size());
        return false;
    }

    num_sats_ = std::atoi(&line[4]);

    for (uint8_t i = 0; i < 17; ++i)
    {
        // Consider it invalid if we have two spaces and a zero.
        if (line[9 + (i * 3)] == ' ')
        {
            continue;
        }

        sats_.emplace_back(&line[9 + (i * 3)], 3);
        SPDLOG_DEBUG("Sat {} -> {}", i, sats_.back());
    }

    return true;
}

bool SP3::handle_lines3_6(const std::string& line)
{
    // SP3 Line Four

    // Columns             Description        Example             Format
    // -----------------   -----------------  -----------------   ---------
    // Columns 1-2         Symbols            +_                  A2
    // Columns 3-9         Unused             _______             7 blanks
    // Columns 10-12       Sat #18 Id         G23                 A1,I2.2
    // Columns 13-15       Sat #19 Id         G24                 A1,I2.2
    //        *
    //        *
    //        *
    // Columns 58-60       Sat #34 Id         __0                 A1,I2.2

    if (line.size() != 60)
    {
        SPDLOG_ERROR("Invalid line length of {}, expected 60.", line.size());
        return false;
    }

    for (uint8_t i = 0; i < 17; ++i)
    {
        // Consider it invalid if we have two spaces and a zero.
        if (line[9 + (i * 3)] == ' ')
        {
            continue;
        }

        sats_.emplace_back(&line[9 + (i * 3)], 3);
        SPDLOG_DEBUG("Sat {} -> {}", i, sats_.back());
    }

    return true;
}

bool SP3::handle_lines7_12(const std::string& line)
{
    // SP3 Line Eight
    // Columns             Description        Example             Format
    // -----------------   -----------------  -----------------   ---------
    // Columns 1-2         Symbols            ++                  A2
    // Columns 3-9         Unused             _______             7 blanks
    // Columns 10-12       Sat #1 Accuracy    __7                 I3
    // Columns 13-15       Sat #2 Accuracy    __8                 I3
    //        *
    //        *
    //        *
    // Columns 58-60       Sat #17 Accuracy   __9                 I3

    if (line.size() != 60)
    {
        SPDLOG_ERROR("Invalid line length of {}, expected 60.", line.size());
        return false;
    }

    // Naive assumption.  Only go until we have the same number as the sats vector.
    if (sats_.size() <= sats_accuracy_.size())
    {
        return true;
    }

    for (uint8_t i = 0; i < 17; ++i)
    {
        uint8_t accuracy = std::atoi(&line[9 + (i * 3)]);
        if (accuracy == 0)
        {
            break;
        }

        sats_accuracy_.push_back(accuracy);
        SPDLOG_DEBUG("Sat Accuracy {} -> {}", i, sats_accuracy_.back());
    }

    return true;
}


}  // namespace parsers
