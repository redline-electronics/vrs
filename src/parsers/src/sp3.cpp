// ftp://igs.org/pub/data/format/sp3c.txt

#include <parsers/sp3.h>
#include <spdlog/spdlog.h>

#include <fstream>

namespace parsers
{

SP3::SP3(const std::string& filename) :
    valid_{false},
    version_{SP3Version::UNKNOWN}
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

    // Columns 1-2.
    if ((line[0] == '#') && (line[1] == 'c'))
    {
        version_ = SP3Version::VERSION_C;
    }
    else
    {
        SPDLOG_ERROR("Unknown version `{}`.", line[1]);
        return false;
    }

    return true;
}

}  // namespace parsers
