#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

uint64_t ByteStream::cap(){
  return capacity_;
}
void Writer::push( string data )
{
  uint64_t len = data.length();

  if(len > capacity_ - buffer_.size()){
    len = capacity_ - buffer_.size();
  }

  for(uint64_t i=0;i<len;i++){
    buffer_.push_back(data[i]);
  }
  totally_pushed_ += len;
  really_pushed = len;
}

void Writer::close(){ write_closed = true; }

void Writer::set_error(){ suffer_error = true; }

bool Writer::is_closed() const { return write_closed; }

uint64_t Writer::available_capacity() const
{
  uint ava_cap = capacity_-buffer_.size();
  return {ava_cap};
}
uint64_t Writer::bytes_pushed() const
{ 
  return totally_pushed_ ;
}

string_view Reader::peek() const
{
  string_view sv1(&buffer_.front(),1);
  return {sv1};
}

void Reader::pop( uint64_t len )
{
  if(len > buffer_.size()){
    len = buffer_.size();
  }
  totally_popped_ += len;
  for(uint64_t i =0;i<len;i++){
    buffer_.pop_front();
  }
  return ;

}
bool Reader::is_finished() const{return (write_closed&&buffer_.size()==0) ? true:false;}

bool Reader::has_error() const{ return suffer_error ;}

uint64_t Reader::bytes_buffered() const{ return  buffer_.size() ;}

uint64_t Reader::bytes_popped() const{ return  totally_popped_ ;}
