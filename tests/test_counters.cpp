#include <aether/metrics/counters.hpp>
#include <iostream>

namespace {
int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}
bool all_zero(const aether::metrics::BrokerMetricsSnapshot& s) {
    return s.published == 0 && s.consumed == 0 && s.publish_failed_full == 0 &&
           s.consume_failed_empty == 0 && s.publish_failed_invalid == 0 &&
           s.consume_failed_invalid == 0 && s.wal_bytes_written == 0 &&
           s.wal_records_written == 0 && s.wal_flushes == 0 && s.wal_failures == 0 &&
           s.recovered_records == 0 && s.recovery_failures == 0;
}
} // namespace
int main() {
    int failures = 0;
    aether::metrics::BrokerCounters counters;
    failures += check(all_zero(counters.snapshot()), "default snapshot is zero");
    counters.record_published();
    counters.record_consumed();
    counters.record_publish_failed_full();
    counters.record_consume_failed_empty();
    counters.record_publish_failed_invalid();
    counters.record_consume_failed_invalid();
    counters.record_wal_write(40);
    counters.record_wal_write(2);
    counters.record_wal_flush();
    counters.record_wal_failure();
    counters.record_recovered_record();
    counters.record_recovery_failure();
    const auto snap = counters.snapshot();
    failures += check(snap.published == 1, "published increments");
    failures += check(snap.consumed == 1, "consumed increments");
    failures += check(snap.publish_failed_full == 1, "full increments");
    failures += check(snap.consume_failed_empty == 1, "empty increments");
    failures += check(snap.publish_failed_invalid == 1, "publish invalid increments");
    failures += check(snap.consume_failed_invalid == 1, "consume invalid increments");
    failures += check(snap.wal_records_written == 2, "WAL records accumulate");
    failures += check(snap.wal_bytes_written == 42, "WAL bytes accumulate");
    failures += check(snap.wal_flushes == 1, "flush increments");
    failures += check(snap.wal_failures == 1, "WAL failure increments");
    failures += check(snap.recovered_records == 1, "recovered increments");
    failures += check(snap.recovery_failures == 1, "recovery failure increments");
    counters.record_published();
    failures += check(snap.published == 1, "snapshot is immutable value copy");
    counters.reset();
    failures += check(all_zero(counters.snapshot()), "reset clears counters");
    if (failures != 0)
        return 1;
    std::cout << "counters test passed\n";
    return 0;
}
