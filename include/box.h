#ifndef BOX_H
#define BOX_H

class box {
   private:
      uint32_t xmin_;
      uint32_t ymin_;
      uint32_t xmax_;
      uint32_t ymax_;

      uint32_t area_;

   public:
      box(uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax) :
            xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax) {
         area_ = (xmax_ - xmin_) * (ymax_ - ymin_); };

      uint32_t area() { return area_; }

      uint32_t xmin() { return xmin_; }
      uint32_t ymin() { return ymin_; }
      uint32_t xmax() { return xmax_; }
      uint32_t ymax() { return ymax_; }
};

#endif /* BOX_H */
