#pragma once

#include "../BasicRenderer.hpp"

struct interrupt_frame;
__attribute__((interrupt)) void PageFault(struct interrupt_frame* frame);
