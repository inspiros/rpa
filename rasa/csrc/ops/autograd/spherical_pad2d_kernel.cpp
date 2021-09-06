#include "../spherical_pad2d.h"

#include <torch/autograd.h>
#include <torch/types.h>

namespace rasa {
    namespace ops {
        namespace {
            class SphericalPad2dFunction
                    : public torch::autograd::Function<SphericalPad2dFunction> {
            public:
                static torch::autograd::variable_list forward(
                        torch::autograd::AutogradContext *ctx,
                        const torch::autograd::Variable &input,
                        int64_t pad_l,
                        int64_t pad_r,
                        int64_t pad_u,
                        int64_t pad_d,
                        const std::string &interpolation) {
                    at::AutoNonVariableTypeMode g;
                    auto output = spherical_pad2d(
                            input,
                            pad_l,
                            pad_r,
                            pad_u,
                            pad_d,
                            interpolation);

                    ctx->save_for_backward({input});
//                    at::IntArrayRef input_shape = input.sizes();
//                    ctx->saved_data["input_shape"] = input_shape;
                    ctx->saved_data["pad_l"] = pad_l;
                    ctx->saved_data["pad_r"] = pad_r;
                    ctx->saved_data["pad_u"] = pad_u;
                    ctx->saved_data["pad_d"] = pad_d;
                    ctx->saved_data["interpolation"] = interpolation;

                    return {
                            output,
                    };
                }

                static torch::autograd::variable_list backward(
                        torch::autograd::AutogradContext *ctx,
                        const torch::autograd::variable_list &grad_output) {

                    auto saved = ctx->get_saved_variables();
                    auto input = saved[0];
//                    at::IntArrayRef input_shape = ctx->saved_data["input_shape"].toIntVector();
                    auto pad_l = ctx->saved_data["pad_l"].toInt();
                    auto pad_r = ctx->saved_data["pad_r"].toInt();
                    auto pad_u = ctx->saved_data["pad_u"].toInt();
                    auto pad_d = ctx->saved_data["pad_d"].toInt();
                    auto interpolation = ctx->saved_data["interpolation"].toString()->string();

                    auto grad_input = detail::_spherical_pad2d_backward(
                            grad_output[0],
                            input,
                            pad_l,
                            pad_r,
                            pad_u,
                            pad_d,
                            interpolation);

                    return {
                            grad_input,
                            torch::autograd::Variable(),
                            torch::autograd::Variable(),
                            torch::autograd::Variable(),
                            torch::autograd::Variable(),
                            torch::autograd::Variable(),
                    };
                }
            };

            class SphericalPad2dBackwardFunction
                    : public torch::autograd::Function<SphericalPad2dBackwardFunction> {
            public:
                static torch::autograd::variable_list forward(
                        torch::autograd::AutogradContext *ctx,
                        const torch::autograd::Variable &grad_output,
                        const torch::autograd::Variable &input,
                        int64_t pad_l,
                        int64_t pad_r,
                        int64_t pad_u,
                        int64_t pad_d,
                        const std::string &interpolation) {
                    at::AutoNonVariableTypeMode g;
                    auto grad_input = detail::_spherical_pad2d_backward(
                            grad_output,
                            input,
                            pad_l,
                            pad_r,
                            pad_u,
                            pad_d,
                            interpolation);

                    ctx->save_for_backward({input});
//                    ctx->saved_data["input_shape"] = input_shape;
                    ctx->saved_data["pad_l"] = pad_l;
                    ctx->saved_data["pad_r"] = pad_r;
                    ctx->saved_data["pad_u"] = pad_u;
                    ctx->saved_data["pad_d"] = pad_d;
                    ctx->saved_data["interpolation"] = interpolation;

                    return {
                            grad_input,
                    };
                }

                static torch::autograd::variable_list backward(
                        torch::autograd::AutogradContext *ctx,
                        const torch::autograd::variable_list &grad_output) {
                    TORCH_CHECK(0, "double backwards on spherical_pad2d not supported");
                }
            };
        } // namespace

        at::Tensor spherical_pad2d_autograd(
                const at::Tensor &input,
                int64_t pad_l,
                int64_t pad_r,
                int64_t pad_u,
                int64_t pad_d,
                const std::string &interpolation
        ) {
            return SphericalPad2dFunction::apply(
                    input,
                    pad_l,
                    pad_r,
                    pad_u,
                    pad_d,
                    interpolation
            )[0];
        }

        at::Tensor spherical_pad2d_backward_autograd(
                const at::Tensor &grad,
                const at::Tensor &input,
                int64_t pad_l,
                int64_t pad_r,
                int64_t pad_u,
                int64_t pad_d,
                const std::string &interpolation
        ) {
            return SphericalPad2dBackwardFunction::apply(
                    grad,
                    input,
                    pad_l,
                    pad_r,
                    pad_u,
                    pad_d,
                    interpolation
            )[0];
        }

        TORCH_LIBRARY_IMPL(rasa, Autograd, m) {
            m.impl(
                    TORCH_SELECTIVE_NAME("rasa::spherical_pad2d"),
                    TORCH_FN(spherical_pad2d_autograd));
            m.impl(
                    TORCH_SELECTIVE_NAME("rasa::_spherical_pad2d_backward"),
                    TORCH_FN(spherical_pad2d_backward_autograd));
        }
    }
}
