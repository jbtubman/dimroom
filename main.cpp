#include <print>
#include <vector>
#include <format>
#include <regex>

#include "contains.hpp"
#include "coordinates.hpp"
#include "coordinate_formatter.hpp"
#include "cell_types.hpp"

int main() {
  std::println("Hello, GeoLOGIC Systems!");

  // auto dmls{u8"deg_min_lat_s = \"{}\""};
  // using dmls_type = decltype(dmls);
  // const auto& dmls_r = dmls;
  // using dmls_r_type = decltype(dmls_r);
  // std::basic_format_string<dmls_r_type> fmt{dmls_r};
  std::println("decimal_lat_s = \"{}\"", jt::decimal_lat_s);

  std::string lat{"-51.05011"};
  auto match_begin = std::sregex_iterator(lat.begin(), lat.end(), jt::decimal_lat_rx);
  auto match_end = std::sregex_iterator();

  if (match_begin != match_end) {
    std::println("found match: {}", match_begin->str());
    float lat_f = std::stof(match_begin->str());
    std::println("latitude = {}", lat_f);
  }
  else {
    std::println("match not found");
  }


  std::string dec_coord{"-51.05011, -114.08529"};
  auto dec_begin = std::sregex_token_iterator(dec_coord.begin(), dec_coord.end(), jt::decimal_coordinate_rx, 2);
  auto dec_end = std::sregex_token_iterator();

  while (dec_begin != dec_end) {
    std::println("found_match: {}", dec_begin->str());
    dec_begin++;
  }

  auto my_coord = jt::make_coordinate(dec_coord);

  std::println("my_coord: {}", my_coord);

  my_coord.coordinate_format = jt::coordinate::format::degrees_minutes;

  std::println("my_coord: {}", my_coord);

  my_coord.coordinate_format = jt::coordinate::format::invalid;

  std::println("my_coord: {}", my_coord);

  // coordinate c1{0.0, 0.0};
  // coordinate c2{10.0, 0.0};
  // coordinate c3{10.0, 10.0};
  // coordinate c4{0.0, 10.0};
  // std::vector vertexes{c1, c2, c3, c4};

  // coordinate inside{5.0, 5.0};
  // coordinate outside{12.0, 12.0};

  // using ContainerType = decltype(vertexes);

  // static_assert(IsRandomAccess<ContainerType>);

  // bool should_be_true = point_in_poly2(vertexes, inside);

  // std::println("should_be_true = {}", should_be_true);

  // bool should_be_false = point_in_poly2(vertexes, outside);

  // std::println("should_be_false = {}", should_be_false);

  return 0;
}
