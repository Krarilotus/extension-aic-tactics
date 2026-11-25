#ifndef AIC_TACTICS_DIAGNOSTICS_HPP
#define AIC_TACTICS_DIAGNOSTICS_HPP

namespace AicTactics {
namespace SHC141 {

// Module option `diagnostics`: lord hunt, ally and patrol decisions are
// written to the UCP log while it is 1.
extern int diagnosticsEnabled;
void diagnostic(const char* format, ...);

} // namespace SHC141
} // namespace AicTactics
#endif
