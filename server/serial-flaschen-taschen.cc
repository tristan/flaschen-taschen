#include "led-flaschen-taschen.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

static int reliable_write(int fd, const uint8_t *buf, size_t size) {
    int written = 0;
    while (size && (written = write(fd, buf, size)) > 0) {
        size -= written;
        buf += written;
    }
    return written;
}

SerialFlaschenTaschen::SerialFlaschenTaschen(int fd, int width, int height)
  : serial_fd_(fd), width_(width), height_(height), is_first_(true),
    last_time_usec_(-1) {

  // buffer = new
  size_ = 1 + (width_ * height_ * 3);
  buffer_ = new uint8_t[size_];
  // start of frame value
  buffer_[0] = 1;
  for (int i = 0; i < width_ * height_; i+= 1) {
    int off = 1 + (i * 3);
    buffer_[off] = 0; // r
    buffer_[off + 1] = 0; // g
    buffer_[off + 2] = 0; // b
  }
}

SerialFlaschenTaschen::~SerialFlaschenTaschen() {
  delete[] buffer_;
}

void SerialFlaschenTaschen::PostDaemonInit() {
}

// used because we're not sure how the glediator protocol is supposed
// to differentiate colour values of 1 from the start of frame marker
// so if the value is one, we just set it to two.
static inline uint8_t ensure_not_one(uint8_t val) {
  return val == 1 ? 2 : val;
}

void SerialFlaschenTaschen::SetPixel(int x, int y, const Color &col) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    // horizontal mirror so that bottom left is 0,0
    y = height_ - (y + 1);
    int pos = 1 + ((y * width_ + x) * 3); // * 3 for rgb
    buffer_[pos] = ensure_not_one(col.r);
    buffer_[pos + 1] = ensure_not_one(col.g);
    buffer_[pos + 2] = ensure_not_one(col.b);
}

void SerialFlaschenTaschen::Send() {
  int written = reliable_write(serial_fd_, buffer_, size_);
  if (written == -1) {
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
  }
}
