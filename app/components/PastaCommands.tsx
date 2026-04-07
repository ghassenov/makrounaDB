import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'

const standardCommands = [
  { cmd: 'PING [message]', desc: 'Test connection, returns PONG' },
  { cmd: 'ECHO message', desc: 'Echo back the given string' },
  { cmd: 'SET key value', desc: 'Set a key to a string value' },
  { cmd: 'GET key', desc: 'Get the value of a key' },
  { cmd: 'DEL key [key ...]', desc: 'Delete one or more keys' },
  { cmd: 'EXISTS key [key ...]', desc: 'Test for key existence' },
  { cmd: 'INCR key', desc: 'Increment integer value by 1' },
  { cmd: 'EXPIRE key seconds', desc: 'Set expiration in seconds' },
  { cmd: 'TTL key', desc: 'Get remaining time-to-live' },
]

const pastaCommands = [
  { cmd: 'RIGATONI key', equiv: 'DECR', desc: 'Decrement integer by 1' },
  { cmd: 'LINGUINE key amount', equiv: 'INCRBY', desc: 'Increment integer by N' },
  { cmd: 'VERMICELLI key amount', equiv: 'DECRBY', desc: 'Decrement integer by N' },
  { cmd: 'SPAGHETTI key', equiv: 'STRLEN', desc: 'Return byte length of value' },
  { cmd: 'PENNE key value', equiv: 'SETNX', desc: 'Set only if key does not exist' },
  { cmd: 'ALDENTE key', equiv: 'PERSIST', desc: 'Remove TTL so key never expires' },
  { cmd: 'FARFALLE key', equiv: 'GETDEL', desc: 'Return value and delete atomically' },
  { cmd: 'LASAGNA key newkey', equiv: 'RENAME', desc: 'Rename key to newkey' },
]

export default function PastaCommands() {
  return (
    <Section id="commands">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // command reference
          </p>
        </FadeIn>

        <FadeIn delay={0.08}>
          <h2
            className="font-heading text-4xl md:text-[2.8rem] font-bold leading-tight mb-3"
            style={{ color: 'var(--text)' }}
          >
            Two flavors of commands.
          </h2>
          <p className="font-body text-xl italic mb-16" style={{ color: 'var(--text-muted)' }}>
            Standard RESP2, and a pasta-themed twist.
          </p>
        </FadeIn>

        <div className="grid lg:grid-cols-2 gap-12 lg:gap-20">
          {/* Standard commands */}
          <div>
            <FadeIn delay={0.1}>
              <h3
                className="font-heading text-xl font-semibold mb-2"
                style={{ color: 'var(--text)' }}
              >
                Standard Commands
              </h3>
              <p className="font-body text-sm mb-7" style={{ color: 'var(--text-muted)' }}>
                Redis-compatible RESP2 commands. Works with any existing client.
              </p>
            </FadeIn>

            <div className="space-y-0.5">
              {standardCommands.map((item, i) => (
                <FadeIn key={item.cmd} delay={0.12 + i * 0.04}>
                  <div className="hover-cmd flex items-start gap-4 px-4 py-2.5 cursor-default">
                    <code
                      className="font-mono text-[12px] px-2 py-0.5 rounded shrink-0 whitespace-nowrap"
                      style={{
                        color: 'var(--gold)',
                        background: 'rgba(201,135,42,0.08)',
                      }}
                    >
                      {item.cmd}
                    </code>
                    <span
                      className="font-body text-sm pt-0.5 hover-muted-to-text"
                      style={{ color: 'var(--text-muted)' }}
                    >
                      {item.desc}
                    </span>
                  </div>
                </FadeIn>
              ))}
            </div>
          </div>

          {/* Pasta commands */}
          <div>
            <FadeIn delay={0.14} direction="left">
              <div className="flex items-center gap-3 mb-2">
                <h3
                  className="font-heading text-xl font-semibold"
                  style={{ color: 'var(--text)' }}
                >
                  Pasta Commands
                </h3>
                <span className="text-2xl" role="img" aria-label="pasta">🍝</span>
              </div>
              <p className="font-body text-sm mb-7" style={{ color: 'var(--text-muted)' }}>
                Because life is too short for boring command names. Fully functional
                aliases with a culinary twist — all of them are real, working commands.
              </p>
            </FadeIn>

            <div className="space-y-0.5">
              {pastaCommands.map((item, i) => (
                <FadeIn key={item.cmd} delay={0.16 + i * 0.05} direction="left">
                  <div className="hover-pasta flex items-start gap-3 px-4 py-2.5 rounded-lg cursor-default">
                    <div className="shrink-0">
                      <code
                        className="font-mono text-[12px] px-2 py-0.5 rounded block whitespace-nowrap mb-0.5"
                        style={{
                          color: 'var(--gold-bright)',
                          background: 'rgba(240,178,67,0.08)',
                        }}
                      >
                        {item.cmd}
                      </code>
                      <span
                        className="font-mono text-[10px] block text-center"
                        style={{ color: 'rgba(139,133,128,0.45)' }}
                      >
                        ≡ {item.equiv}
                      </span>
                    </div>
                    <span
                      className="font-body text-sm pt-0.5"
                      style={{ color: 'var(--text-muted)' }}
                    >
                      {item.desc}
                    </span>
                  </div>
                </FadeIn>
              ))}
            </div>
          </div>
        </div>

        {/* Protocol note */}
        <FadeIn delay={0.3}>
          <div
            className="mt-14 p-6 rounded-xl"
            style={{
              border: '1px solid rgba(255,255,255,0.06)',
              background: 'rgba(255,255,255,0.015)',
            }}
          >
            <p className="font-mono text-[12px] leading-relaxed" style={{ color: 'var(--text-muted)' }}>
              <span style={{ color: 'var(--gold)' }}>RESP2 format: </span>
              {`*<argc>\\r\\n$<len>\\r\\n<arg>\\r\\n...`}
              {'  ·  '}
              <span style={{ color: 'var(--green)' }}>+</span> Simple string
              {'  ·  '}
              <span style={{ color: 'var(--green)' }}>:</span> Integer
              {'  ·  '}
              <span style={{ color: 'var(--green)' }}>$</span> Bulk string
              {'  ·  '}
              <span style={{ color: 'var(--red)' }}>-</span> Error
            </p>
          </div>
        </FadeIn>
      </div>
    </Section>
  )
}
