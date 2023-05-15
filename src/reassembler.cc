#include "reassembler.hh"
#include <iostream>
using namespace std;

long Reassembler::merge_block(block_node &elm1, const block_node &elm2) {
    block_node x, y;
    if (elm1.begin > elm2.begin) {
        x = elm2;
        y = elm1;
    } else {
        x = elm1;
        y = elm2;
    }
    if (x.begin + x.length < y.begin) {
        return -1;  // no intersection, couldn't merge
    } else if (x.begin + x.length >= y.begin + y.length) {
        elm1 = x;
        return y.length;
    } else {
        elm1.begin = x.begin;
        elm1.data = x.data + y.data.substr(x.begin + x.length - y.begin);
        elm1.length = elm1.data.length();
        return x.begin + x.length - y.begin;
    }
}
uint64_t Reassembler::sum_set(){
    uint64_t len = 0;
    auto iter = _blocks.begin();
    while(iter!=_blocks.end()){
        iter++;
        len += iter->length;
    }
    return len;
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
    if(is_EOF ==false && is_last_substring==true){
        is_EOF = true;
    }
    if (first_index >= _head_index + output.available_capacity()) {  // capacity over
            return;
        }
    // handle extra substring prefix
    block_node elm;
    if (first_index + data.length() <= _head_index) {  // couldn't equal, because there have empty substring
        goto JUDGE_EOF;
    }else if (first_index < _head_index) {
        uint64_t offset = _head_index - first_index;
        elm.data.assign(data.begin() + offset, data.end());
        elm.begin = first_index + offset;
        elm.length = elm.data.length();
        if(elm.length>output.available_capacity()){
            // elm.data.assign(elm.data.begin(), elm.data.end()-(elm.data.length()-output.available_capacity()+sum_set()));
            elm.data.assign(elm.data.begin(), elm.data.end()-(elm.data.length()-output.available_capacity()));
            elm.length = elm.data.length();
        }
    } else {
        elm.begin = first_index;
        elm.length = data.length();
        elm.data = data;
        if(elm.length>output.available_capacity()){
            // elm.data.assign(elm.data.begin(), elm.data.end()-(elm.data.length()-output.available_capacity()+sum_set()));
            elm.data.assign(elm.data.begin(), elm.data.end()-(elm.data.length()-output.available_capacity()));
            elm.length = elm.data.length();
        }
    }
    // if(1){
    //   return;
    // }
    _unassembled_byte += elm.length;

    // merge substring
    do {
        // merge next
        long merged_bytes = 0;
        auto iter = _blocks.lower_bound(elm);
        while (iter != _blocks.end() && (merged_bytes = merge_block(elm, *iter)) >= 0) {
            _unassembled_byte -= merged_bytes;
            _blocks.erase(iter);
            iter = _blocks.lower_bound(elm);
        }
        // merge prev
        if (iter == _blocks.begin()) {
            break;
        }
        iter--;
        while ((merged_bytes = merge_block(elm, *iter)) >= 0) {
            _unassembled_byte -= merged_bytes;
            _blocks.erase(iter);
            iter = _blocks.lower_bound(elm);
            if (iter == _blocks.begin()) {
                break;
            }
            iter--;
        }
    } while (false);
    _blocks.insert(elm);
    
    // write to ByteStreamdsad
    if (!_blocks.empty() && _blocks.begin()->begin == _head_index) {
        const block_node head_block = *_blocks.begin();
        // modify _head_index and _unassembled_byte according to successful write to _output
        uint64_t write_bytes = head_block.data.length();
        output.push(head_block.data);
        _head_index += write_bytes;
        // _head_index = 18;
        // cout<<_head_index<<endl;
        _unassembled_byte -= write_bytes;
        _blocks.erase(_blocks.begin());
    }

    goto JUDGE_EOF;


JUDGE_EOF:
    if (is_EOF&& bytes_pending()==0) {
        output.close();
    }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return _unassembled_byte;
}
