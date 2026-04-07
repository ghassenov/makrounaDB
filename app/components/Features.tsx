import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'

const features = [
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <path d="M8.5 14.5A2.5 2.5 0 0011 12c0-1.38-.5-2-1-3-1.072-2.143-.224-4.054 2-6 .5 2.5 2 4.9 4 6.5 2 1.6 3 3.5 3 5.5a7 7 0 11-14 0c0-1.153.433-2.294 1-3a2.5 2.5 0 002.5 2.5z" />
      </svg>
    ),
    title: 'RESP2 Protocol',
    description:
      'Fully compatible with the Redis Serialization Protocol v2. Connect with any Redis client — redis-cli, Jedis, ioredis — without any code changes.',
  },
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <circle cx="12" cy="12" r="10" />
        <polyline points="12 6 12 12 16 14" />
      </svg>
    ),
    title: 'TTL Expiration',
    description:
      'Set time-to-live on any key with EXPIRE. Lazy expiration on access, with periodic bounded cleanup run from the event loop.',
  },
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <path d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4" />
        <polyline points="7 10 12 15 17 10" />
        <line x1="12" y1="15" x2="12" y2="3" />
      </svg>
    ),
    title: 'AOF Persistence',
    description:
      'Every write is appended to an AOF log in RESP-array format. On restart, the log is replayed through the same command dispatcher — zero data loss.',
  },
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <path d="M21 15a2 2 0 01-2 2H7l-4 4V5a2 2 0 012-2h14a2 2 0 012 2z" />
      </svg>
    ),
    title: 'Natural Language CLI',
    description:
      'Ask your database in plain English. "Set user to ghassen", "What is user?", "Expire user in 30 seconds" — the NL shell translates to RESP2.',
  },
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <path d="M12 2l3.09 6.26L22 9.27l-5 4.87 1.18 6.88L12 17.77l-6.18 3.25L7 14.14 2 9.27l6.91-1.01L12 2z" />
      </svg>
    ),
    title: 'Pasta Commands',
    description:
      'Unique pasta-themed aliases: RIGATONI (DECR), LINGUINE (INCRBY), PENNE (SETNX), LASAGNA (RENAME), FARFALLE (GETDEL)… all real commands.',
  },
  {
    icon: (
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <rect x="2" y="3" width="20" height="14" rx="2" ry="2" />
        <line x1="8" y1="21" x2="16" y2="21" />
        <line x1="12" y1="17" x2="12" y2="21" />
      </svg>
    ),
    title: 'Event-Loop Server',
    description:
      'Single-process TCP server using select() for readiness-based socket handling. Client input buffered and parsed incrementally per connection.',
  },
]

export default function Features() {
  return (
    <Section id="features">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // features
          </p>
        </FadeIn>

        <FadeIn delay={0.08}>
          <h2
            className="font-heading text-4xl md:text-[2.8rem] font-bold leading-tight mb-4"
            style={{ color: 'var(--text)' }}
          >
            Everything you need.
          </h2>
          <p className="font-body text-xl italic mb-16" style={{ color: 'var(--text-muted)' }}>
            Nothing you don&apos;t.
          </p>
        </FadeIn>

        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-5">
          {features.map((feature, i) => (
            <FadeIn key={feature.title} delay={i * 0.07}>
              <div
                className="hover-card p-7 rounded-2xl h-full cursor-default"
                style={{
                  border: '1px solid rgba(255,255,255,0.07)',
                  background: 'rgba(255,255,255,0.02)',
                }}
              >
                {/* Icon */}
                <div
                  className="w-10 h-10 rounded-xl flex items-center justify-center mb-5"
                  style={{
                    border: '1px solid rgba(201,135,42,0.2)',
                    background: 'rgba(201,135,42,0.07)',
                    color: 'var(--gold)',
                  }}
                >
                  {feature.icon}
                </div>

                <h3
                  className="font-heading text-[1.05rem] font-semibold mb-3"
                  style={{ color: 'var(--text)' }}
                >
                  {feature.title}
                </h3>
                <p
                  className="font-body text-[0.95rem] leading-relaxed"
                  style={{ color: 'var(--text-muted)' }}
                >
                  {feature.description}
                </p>
              </div>
            </FadeIn>
          ))}
        </div>
      </div>
    </Section>
  )
}
