#ifndef BOX_H
#define BOX_H

class box {
   private:
      uint32_t xmin_;
      uint32_t ymin_;
      uint32_t xmax_;
      uint32_t ymax_;

      uint32_t size_;

   public:
      box(uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax) :
            xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax) {
         size_ = (xmax_ - xmin_) * (ymax_ - ymin_); };

      uint32_t xmin() const { return xmin_; }
      uint32_t ymin() const { return ymin_; }
      uint32_t xmax() const { return xmax_; }
      uint32_t ymax() const { return ymax_; }

      uint32_t size() const { return size_; }
};

#endif /* BOX_H */
