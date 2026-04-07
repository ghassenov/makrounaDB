import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'

const pipeline = [
  {
    label: 'Network Layer',
    detail: 'TCP server · select() event loop',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <rect x="2" y="2" width="20" height="8" rx="2" ry="2" />
        <rect x="2" y="14" width="20" height="8" rx="2" ry="2" />
        <line x1="6" y1="6" x2="6.01" y2="6" />
        <line x1="6" y1="18" x2="6.01" y2="18" />
      </svg>
    ),
  },
  {
    label: 'Protocol Parser',
    detail: 'Incremental RESP2 frame parsing',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <polyline points="16 18 22 12 16 6" />
        <polyline points="8 6 2 12 8 18" />
      </svg>
    ),
  },
  {
    label: 'Command Dispatcher',
    detail: 'Routing + command semantics',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <circle cx="12" cy="12" r="3" />
        <path d="M12 1v4M12 19v4M4.22 4.22l2.83 2.83M16.95 16.95l2.83 2.83M1 12h4M19 12h4M4.22 19.78l2.83-2.83M16.95 7.05l2.83-2.83" />
      </svg>
    ),
  },
  {
    label: 'Storage + TTL',
    detail: 'unordered_map · expiration metadata · lazy cleanup',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <ellipse cx="12" cy="5" rx="9" ry="3" />
        <path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3" />
        <path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5" />
      </svg>
    ),
  },
  {
    label: 'AOF Persistence',
    detail: 'Append-only log · everysec flush · replay on startup',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z" />
        <polyline points="14 2 14 8 20 8" />
        <line x1="16" y1="13" x2="8" y2="13" />
        <line x1="16" y1="17" x2="8" y2="17" />
      </svg>
    ),
  },
  {
    label: 'RESP Reply',
    detail: 'Encoded response back to the client',
    icon: (
      <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5">
        <line x1="22" y1="2" x2="11" y2="13" />
        <polygon points="22 2 15 22 11 13 2 9 22 2" />
      </svg>
    ),
  },
]

const modules = [
  { path: 'src/app', desc: 'CLI entrypoint and server lifecycle' },
  { path: 'src/network', desc: 'TCP server and event loop' },
  { path: 'src/protocol', desc: 'RESP request parser and reply encoder' },
  { path: 'src/command', desc: 'Command routing and semantics' },
  { path: 'src/storage', desc: 'Keyspace and TTL behavior' },
  { path: 'src/persistence', desc: 'Append-only file persistence' },
]

const roadmap = [
  'Parser hardening for malformed frames and large payloads',
  'Strict RESP2 error parity matrix against Redis',
  'Worker-pool-backed background maintenance',
  'Optional snapshot persistence mode',
]

export default function Architecture() {
  return (
    <Section id="architecture">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // architecture
          </p>
        </FadeIn>

        <FadeIn delay={0.08}>
          <h2
            className="font-heading text-4xl md:text-[2.8rem] font-bold mb-4"
            style={{ color: 'var(--text)' }}
          >
            Simple. Fast. Transparent.
          </h2>
          <p
            className="font-body text-[1.1rem] mb-16 max-w-2xl"
            style={{ color: 'var(--text-muted)' }}
          >
            A single-process server with a clear, linear request path. No hidden magic —
            just C++ doing what it does best.
          </p>
        </FadeIn>

        <div className="grid lg:grid-cols-2 gap-16">
          {/* Request pipeline */}
          <FadeIn delay={0.12}>
            <h3
              className="font-heading text-lg font-semibold mb-8"
              style={{ color: 'var(--text)' }}
            >
              Request Pipeline
            </h3>
            <div>
              {pipeline.map((step, i) => (
                <div key={step.label}>
                  <div className="flex items-center gap-4 py-3">
                    <div
                      className="w-10 h-10 rounded-xl flex items-center justify-center shrink-0"
                      style={{
                        border: '1px solid rgba(201,135,42,0.25)',
                        background: 'rgba(201,135,42,0.06)',
                        color: 'var(--gold)',
                      }}
                    >
                      {step.icon}
                    </div>
                    <div>
                      <div
                        className="font-mono text-[13px] font-medium"
                        style={{ color: 'var(--text)' }}
                      >
                        {step.label}
                      </div>
                      <div className="font-mono text-[11px]" style={{ color: 'var(--text-muted)' }}>
                        {step.detail}
                      </div>
                    </div>
                  </div>
                  {i < pipeline.length - 1 && (
                    <div
                      className="ml-5 w-px h-3"
                      style={{
                        background:
                          'linear-gradient(to bottom, rgba(201,135,42,0.2), rgba(201,135,42,0.06))',
                      }}
                    />
                  )}
                </div>
              ))}
            </div>
          </FadeIn>

          {/* Module map + roadmap */}
          <div>
            <FadeIn delay={0.18} direction="left">
              <h3
                className="font-heading text-lg font-semibold mb-6"
                style={{ color: 'var(--text)' }}
              >
                Module Map
              </h3>
              <div className="space-y-2 mb-10">
                {modules.map((mod) => (
                  <div
                    key={mod.path}
                    className="hover-module flex items-start gap-4 p-4 rounded-xl"
                    style={{
                      border: '1px solid rgba(255,255,255,0.06)',
                      background: 'rgba(255,255,255,0.02)',
                    }}
                  >
                    <code
                      className="font-mono text-[12px] shrink-0 px-2 py-0.5 rounded whitespace-nowrap"
                      style={{
                        color: 'var(--gold)',
                        background: 'rgba(201,135,42,0.08)',
                      }}
                    >
                      {mod.path}
                    </code>
                    <span className="font-body text-sm" style={{ color: 'var(--text-muted)' }}>
                      {mod.desc}
                    </span>
                  </div>
                ))}
              </div>
            </FadeIn>

            <FadeIn delay={0.26} direction="left">
              <div
                className="p-5 rounded-xl"
                style={{
                  border: '1px dashed rgba(255,255,255,0.1)',
                  background: 'rgba(255,255,255,0.01)',
                }}
              >
                <h4
                  className="font-mono text-[11px] uppercase tracking-widest mb-4"
                  style={{ color: 'var(--text-muted)' }}
                >
                  Roadmap
                </h4>
                <ul className="space-y-2">
                  {roadmap.map((item) => (
                    <li key={item} className="flex items-start gap-3">
                      <span
                        className="font-mono text-[11px] mt-0.5 shrink-0"
                        style={{ color: 'rgba(201,135,42,0.4)' }}
                      >
                        ○
                      </span>
                      <span className="font-mono text-[12px]" style={{ color: 'rgba(139,133,128,0.7)' }}>
                        {item}
                      </span>
                    </li>
                  ))}
                </ul>
              </div>
            </FadeIn>
          </div>
        </div>
      </div>
    </Section>
  )
}
