#include "reassembler.hh"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <tuple>

using namespace std;
using namespace std::chrono;


void print(const string & str){
  for(uint64_t i=0;i<str.size();i++){
    cout<<int(str[i])<<' ';
  }
  cout<<"   length= "<<str.length();
  cout<<endl;
}

void speed_test( const size_t num_chunks,   // NOLINT(bugprone-easily-swappable-parameters)
                 const size_t capacity,     // NOLINT(bugprone-easily-swappable-parameters)
                 const size_t random_seed ) // NOLINT(bugprone-easily-swappable-parameters)
{
  // cout<<random_seed;
  // Generate the data to be written
  const string data = [&] {
    default_random_engine rd { random_seed };
    uniform_int_distribution<char> ud;
    string ret;
    for ( size_t i = 0; i < num_chunks * capacity; ++i ) {
      ret += ud( rd );
    }
    return ret;
  }();
  // const string data = "01234567890123456789";
  // Split the data into segments before writing
  queue<tuple<uint64_t, string, bool>> split_data;
  
  for ( size_t i = 0; i < data.size(); i += capacity ) {
    split_data.emplace( i + 2, data.substr( i + 2, capacity * 2 ), i + 2 + capacity * 2 >= data.size() );
    split_data.emplace( i, data.substr( i, capacity * 2 ), i + capacity * 2 >= data.size() );
    split_data.emplace( i + 1, data.substr( i + 1, capacity * 2 ), i + 1 + capacity * 2 >= data.size() );
  }
  queue<tuple<uint64_t, string, bool>> split_data2 = split_data;
  // auto it = split_data2.front();
  // cout<<"--------------------------------------------------------------"<<endl;
  // cout<<"split_data:"<<endl;
  // while(split_data2.empty()==false){
  //   // cout<<"yes"<<endl;
  //   cout<<get<0>(split_data2.front())<<"     ";
  //   print(get<1>(split_data2.front()));
    
  //   split_data2.pop();
  // }
  // cout<<"--------------------------------------------------------------"<<endl;

  ByteStream stream { capacity };
  Reassembler reassembler;

  string output_data;
  output_data.reserve( data.size() );

  const auto start_time = steady_clock::now();
  while ( not split_data.empty() ) {
    auto& next = split_data.front();
    reassembler.insert( get<uint64_t>( next ), move( get<string>( next ) ), get<bool>( next ), stream.writer() );
    split_data.pop();

    while ( stream.reader().bytes_buffered() ) {
      output_data += stream.reader().peek();
      
      stream.reader().pop( output_data.size() - stream.reader().bytes_popped() );
    }
  }
  
  // print(data);
  // print(output_data);
  // cout<<"--------------------------------------------------------------"<<endl;

  const auto stop_time = steady_clock::now();
  // cout<<"--------------------------------------------------------------"<<endl;
  // cout<<"data ={{{"<<data.length()<<"}}}"<<endl;
  // cout<<"--------------------------------------------------------------"<<endl;
  // cout<<"output_data ={{{"<<output_data.length()<<"}}}"<<endl;
  // cout<<"--------------------------------------------------------------"<<endl;
  if ( not stream.reader().is_finished() ) {
    throw runtime_error( "Reassembler did not close ByteStream when finished" );
  }


  
  if ( data != output_data ) {
    throw runtime_error( "Mismatch between data written and read" );
  }

  
  auto test_duration = duration_cast<duration<double>>( stop_time - start_time );
  auto bytes_per_second = static_cast<double>( num_chunks * capacity ) / test_duration.count();
  auto bits_per_second = 8 * bytes_per_second;
  auto gigabits_per_second = bits_per_second / 1e9;

  fstream debug_output;
  debug_output.open( "/dev/tty" );

  cout << "Reassembler to ByteStream with capacity=" << capacity << " reached " << fixed << setprecision( 2 )
       << gigabits_per_second << " Gbit/s.\n";

  debug_output << "             Reassembler throughput: " << fixed << setprecision( 2 ) << gigabits_per_second
               << " Gbit/s\n";

  if ( gigabits_per_second < 0.1 ) {
    throw runtime_error( "Reassembler did not meet minimum speed of 0.1 Gbit/s." );
  }
}


void program_body()
{
  speed_test( 10000, 1500, 1370 );
  
}
int main()
{
  try {
    program_body();
  } catch ( const exception& e ) {
    cerr << "Exception: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
