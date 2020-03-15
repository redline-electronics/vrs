#ifndef SP3_H_
#define SP3_H_

#include <cstdint>
#include <string>
#include <vector>

namespace parsers
{

enum class SP3Version
{
    UNKNOWN = 0,
    VERSION_C = 1
};


class SP3
{
  public:
    SP3(const std::string& filename);

    bool valid();

  private:
    bool handle_line0(const std::string& line);

    bool valid_;

    SP3Version version_;
};

}  // namespace parsers

#endif
