import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'

const stats = [
  { value: 'RESP2', label: 'Wire Protocol' },
  { value: 'AOF', label: 'Persistence Mode' },
  { value: 'C++17', label: 'Language Standard' },
  { value: 'MIT', label: 'License' },
]

export default function Overview() {
  return (
    <Section id="overview">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // overview
          </p>
        </FadeIn>

        <div className="grid lg:grid-cols-2 gap-16 lg:gap-28 items-start">
          {/* Left */}
          <div>
            <FadeIn delay={0.08}>
              <h2
                className="font-heading text-4xl md:text-[2.8rem] font-bold leading-tight mb-7"
                style={{ color: 'var(--text)' }}
              >
                A database that speaks Redis,
                <br />
                <span style={{ color: 'var(--gold)' }}>built from first principles.</span>
              </h2>
            </FadeIn>

            <FadeIn delay={0.16}>
              <p
                className="font-body text-[1.1rem] leading-relaxed mb-5"
                style={{ color: 'var(--text-muted)' }}
              >
                makrounaDB is a lightweight, Redis-compatible in-memory key-value store that runs
                as a CLI server. It implements the RESP2 wire protocol — meaning any Redis client
                connects without modification.
              </p>
            </FadeIn>

            <FadeIn delay={0.24}>
              <p
                className="font-body text-[1.1rem] leading-relaxed"
                style={{ color: 'var(--text-muted)' }}
              >
                Written in modern C++, it supports TTL-based key expiration, append-only file
                persistence, and a unique natural language interface — so you can interact with
                your data the way you actually think.
              </p>
            </FadeIn>
          </div>

          {/* Right */}
          <div>
            <FadeIn delay={0.18} direction="left">
              <blockquote
                className="pl-6 mb-10"
                style={{ borderLeft: '2px solid rgba(201,135,42,0.5)' }}
              >
                <p
                  className="font-body text-[1.3rem] italic leading-relaxed"
                  style={{ color: 'var(--text)' }}
                >
                  &ldquo;Sometimes you need a lightweight, embeddable key-value store —
                  without the operational overhead of a full Redis deployment.&rdquo;
                </p>
              </blockquote>
            </FadeIn>

            <FadeIn delay={0.28} direction="left">
              <div className="grid grid-cols-2 gap-3">
                {stats.map((stat) => (
                  <div
                    key={stat.value}
                    className="p-5 rounded-xl hover-stat"
                    style={{
                      border: '1px solid rgba(255,255,255,0.07)',
                      background: 'rgba(255,255,255,0.02)',
                    }}
                  >
                    <div
                      className="font-mono text-2xl font-semibold mb-1"
                      style={{ color: 'var(--gold)' }}
                    >
                      {stat.value}
                    </div>
                    <div
                      className="font-mono text-[11px] tracking-wide uppercase"
                      style={{ color: 'var(--text-muted)' }}
                    >
                      {stat.label}
                    </div>
                  </div>
                ))}
              </div>
            </FadeIn>
          </div>
        </div>
      </div>
    </Section>
  )
}
