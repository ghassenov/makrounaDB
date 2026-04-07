const footerLinks = [
  {
    title: 'Project',
    items: [
      { label: 'GitHub', href: 'https://github.com/ghassenov/makrounaDB' },
      { label: 'Issues', href: 'https://github.com/ghassenov/makrounaDB/issues' },
      { label: 'Releases', href: 'https://github.com/ghassenov/makrounaDB/releases' },
      { label: 'Pull Requests', href: 'https://github.com/ghassenov/makrounaDB/pulls' },
    ],
  },
  {
    title: 'Docs',
    items: [
      { label: 'Architecture', href: '#architecture' },
      { label: 'Command Reference', href: '#commands' },
      {
        label: 'Protocol Spec',
        href: 'https://github.com/ghassenov/makrounaDB/blob/main/docs/protocol/command-spec.md',
      },
      { label: 'Getting Started', href: '#installation' },
    ],
  },
  {
    title: 'Legal',
    items: [
      {
        label: 'MIT License',
        href: 'https://github.com/ghassenov/makrounaDB/blob/main/LICENSE',
      },
    ],
  },
]

export default function Footer() {
  return (
    <footer
      className="px-6 md:px-12 lg:px-24 pt-16 pb-10"
      style={{ borderTop: '1px solid rgba(255,255,255,0.06)' }}
    >
      <div className="max-w-7xl mx-auto">
        <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-12 mb-16">
          {/* Brand */}
          <div>
            <div
              className="font-mono text-base font-semibold tracking-widest uppercase mb-3"
              style={{ color: 'var(--gold)' }}
            >
              makrounaDB
            </div>
            <p
              className="font-body text-sm leading-relaxed mb-5"
              style={{ color: 'var(--text-muted)' }}
            >
              Redis-inspired in-memory database written in modern C++. Open source. MIT licensed.
            </p>
            <div className="flex items-center gap-2">
              <span
                className="font-mono text-[11px] px-2.5 py-1 rounded border"
                style={{
                  color: 'rgba(139,133,128,0.6)',
                  borderColor: 'rgba(255,255,255,0.07)',
                }}
              >
                v0.1.0
              </span>
              <span
                className="font-mono text-[11px] px-2.5 py-1 rounded border"
                style={{
                  color: 'rgba(61,220,132,0.6)',
                  borderColor: 'rgba(61,220,132,0.15)',
                }}
              >
                MIT
              </span>
            </div>
          </div>

          {/* Link groups */}
          {footerLinks.map((group) => (
            <div key={group.title}>
              <h4
                className="font-mono text-[11px] tracking-[0.2em] uppercase mb-5"
                style={{ color: 'var(--text-muted)' }}
              >
                {group.title}
              </h4>
              <ul className="space-y-2.5">
                {group.items.map((item) => (
                  <li key={item.label}>
                    <a
                      href={item.href}
                      target={item.href.startsWith('http') ? '_blank' : undefined}
                      rel={item.href.startsWith('http') ? 'noopener noreferrer' : undefined}
                      className="hover-gold font-body text-sm"
                      style={{ color: 'var(--text-muted)' }}
                    >
                      {item.label}
                    </a>
                  </li>
                ))}
              </ul>
            </div>
          ))}
        </div>

        {/* Bottom bar */}
        <div
          className="pt-8 flex flex-col md:flex-row items-center justify-between gap-4"
          style={{ borderTop: '1px solid rgba(255,255,255,0.05)' }}
        >
          <p className="font-mono text-[11px]" style={{ color: 'rgba(139,133,128,0.4)' }}>
            © {new Date().getFullYear()} makrounaDB. Released under the MIT License.
          </p>

          <div className="flex items-center gap-6">
            <a
              href="https://github.com/ghassenov/makrounaDB"
              target="_blank"
              rel="noopener noreferrer"
              className="hover-gold flex items-center gap-2 font-mono text-[11px]"
              style={{ color: 'rgba(139,133,128,0.4)' }}
            >
              <GitHubIcon />
              ghassenov/makrounaDB
            </a>
            <span className="font-mono text-[11px]" style={{ color: 'rgba(139,133,128,0.35)' }}>
              Made with 🍝 and C++
            </span>
          </div>
        </div>
      </div>
    </footer>
  )
}

function GitHubIcon() {
  return (
    <svg width="12" height="12" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
      <path d="M12 0C5.374 0 0 5.373 0 12c0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23A11.509 11.509 0 0112 5.803c1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576C20.566 21.797 24 17.3 24 12c0-6.627-5.373-12-12-12z" />
    </svg>
  )
}
