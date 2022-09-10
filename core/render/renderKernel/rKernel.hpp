#pragma once

#include "../render.hpp"
#include "dyMath.hpp"
#include <functional>

namespace dym {
namespace rt {

class RKernel {
public:
  RKernel(){};
  ~RKernel(){};
  virtual GBuffer renderGBuffer(const Ray &r, const Hittable &world) {
    GBuffer out_gbuffer;
    HitRecord rec;
    if (!world.hit(r, 0.001, infinity, rec))
      return out_gbuffer;
    ScatterRecord srec;
    ColorRGB emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    out_gbuffer.normal = rec.normal;
    out_gbuffer.position = rec.p;
    out_gbuffer.albedo = emitted;
    out_gbuffer.obj_id = rec.obj_id;
    if (rec.mat_ptr->scatter(r, rec, srec))
      out_gbuffer.albedo += srec.attenuation;
    return out_gbuffer;
  }

  virtual bool endCondition(Real &value) { return value <= 0 ? true : false; }

  virtual ColorRGB render(
      const Ray &r, const Hittable &world, shared_ptr<HittableList> lights,
      Real depth,
      const std::function<ColorRGB(const Ray &r)> &background =
          [](const Ray &r) { return ColorRGB(0.f); }) {
    HitRecord rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (endCondition(depth))
      return ColorRGB(0.f);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
      return background(r);

    ScatterRecord srec;
    ColorRGB emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec))
      return emitted;

    if (srec.is_specular) {
      Real pdf_val_ = 1.;
      Ray scattered_ = srec.specular_ray;
      if (srec.pdf_ptr) {
        Vector3 dir_ = srec.pdf_ptr->generate();
        pdf_val_ = srec.pdf_ptr->value(dir_);
        scattered_.dir = dir_.normalize();
      }
      return emitted +
             srec.attenuation *
                 render(scattered_, world, lights, depth - 1, background) /
                 pdf_val_;
    }

    shared_ptr<pdf> p;
    if (lights && lights->objects.size() > 0) {
      auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
      p = make_shared<mixture_pdf>(light_ptr, srec.pdf_ptr);
    } else
      p = srec.pdf_ptr;

    Ray scattered = Ray(rec.p, p->generate(), r.time());
    auto pdf_val = p->value(scattered.direction());

    return emitted +
           srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
               render(scattered, world, lights, depth - 1, background) /
               pdf_val;
  }
};

} // namespace rt
} // namespace dym