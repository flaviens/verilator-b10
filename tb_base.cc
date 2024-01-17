// Copyright 2023 Flavien Solt, ETH Zurich.
// Licensed under the General Public License, Version 3.0, see LICENSE for details.
// SPDX-License-Identifier: GPL-3.0-only

#include "Vtop.h"
#include "verilated.h"
#include "ticks.h"

#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <cassert>

#if VM_TRACE
#if VM_TRACE_FST
#include <verilated_fst_c.h>
#else
#include <verilated_vcd_c.h>
#endif // VM_TRACE_FST
#endif // VM_TRACE

typedef Vtop Module;

size_t curr_id_in_random_inputs_from_file = 0;
size_t curr_id_in_bundle_id_order = 0;

std::vector<uint32_t> random_inputs_from_file;
std::vector<uint32_t> bundle_id_order;

#if VM_TRACE
const int kTraceLevel = 6;
#if VM_TRACE_FST
  VerilatedFstC *trace_;
#else
  VerilatedVcdC *trace_;
#endif // VM_TRACE_FST
#endif // VM_TRACE

int read_random_inputs_from_file() {
  random_inputs_from_file.push_back(0x0);
  random_inputs_from_file.push_back(0x0);
  random_inputs_from_file.push_back(0x0);
  random_inputs_from_file.push_back(0x0);
  random_inputs_from_file.push_back(0xa95de522);
  random_inputs_from_file.push_back(0x93cb341b);
  random_inputs_from_file.push_back(0x620ef6af);
  random_inputs_from_file.push_back(0x42de459d);
  random_inputs_from_file.push_back(0xd58bd330);
  random_inputs_from_file.push_back(0xefc09765);
  random_inputs_from_file.push_back(0xeb95a18);

  bundle_id_order.push_back(1);
  bundle_id_order.push_back(2);
  bundle_id_order.push_back(3);
  bundle_id_order.push_back(4);
  bundle_id_order.push_back(2);
  bundle_id_order.push_back(1);
  bundle_id_order.push_back(0);
  bundle_id_order.push_back(3);
  bundle_id_order.push_back(1);

  return bundle_id_order.size();
}

void feed_inputs(Module *my_module) {
  int curr_bundle_id = bundle_id_order[curr_id_in_bundle_id_order++];

  if (!curr_bundle_id) {
      for (int word_id_in_input = (curr_bundle_id * 96) / 32; word_id_in_input < ((curr_bundle_id + 1) * 96) / 32; word_id_in_input++) {
        my_module->in_data[word_id_in_input] = random_inputs_from_file[curr_id_in_random_inputs_from_file++];
      }
  } else {
    int curr_clkin_id = curr_bundle_id - 1;
    my_module->clkin_data[curr_clkin_id] = random_inputs_from_file[curr_id_in_random_inputs_from_file++];
  }
}

/**
 * Runs the testbench.
 *
 * @param tb a pointer to a testbench instance
 * @param simlen the number of cycles to run
 */
std::pair<long, uint64_t> run_test(Module *my_module, int simlen, const std::string trace_filename) {
  uint64_t out;
  auto start = std::chrono::steady_clock::now();

#if VM_TRACE
#if VM_TRACE_FST
    trace_ = new VerilatedFstC;
#else
    trace_ = new VerilatedVcdC;
#endif // VM_TRACE_FST
    my_module->trace(trace_, kTraceLevel);
    trace_->open(trace_filename.c_str());
    size_t tick_count_ = 0;
#endif // VM_TRACE

  for (int tick_id = 0; tick_id < simlen; tick_id++) {
    feed_inputs(my_module);
    my_module->eval();
#if VM_TRACE
      trace_->dump(tick_count_++);
#endif // VM_TRACE
  }
  out = my_module->out_data[0];

#if VM_TRACE
      trace_->flush();
#endif // VM_TRACE

  auto stop = std::chrono::steady_clock::now();
  long ret = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
  return std::make_pair(ret, out);
}

int main(int argc, char **argv, char **env) {

  Verilated::commandArgs(argc, argv);
  Verilated::traceEverOn(VM_TRACE);

  ////////
  // Instantiate the module.
  ////////

  Module *my_module = new Module;

  ////////
  // Get the ctx vars.
  ////////

  int simlen = read_random_inputs_from_file();
  std::string vcd_filepath = cl_get_tracefile();

  ////////
  // Run the experiment.
  ////////

  std::pair<long, uint64_t> duration_and_output = run_test(my_module, simlen, vcd_filepath);
  long duration = duration_and_output.first;
  uint64_t out = duration_and_output.second;

  std::cout << "Output: " << out << std::endl;

  delete my_module;
  exit(0);
}
