'use client'

import { useEffect, useState, useRef } from 'react'
import { motion } from 'framer-motion'

interface TerminalEntry {
  text: string
  type: 'cmd' | 'ok' | 'input' | 'resp' | 'blank' | 'comment'
  delay: number
}

const TERMINAL_LINES: TerminalEntry[] = [
  { text: '$ ./build/makrounaDB --port 6379 --aof data/appendonly.aof', delay: 300, type: 'cmd' },
  { text: '', delay: 700, type: 'blank' },
  { text: '  ✓  Listening on 127.0.0.1:6379', delay: 1000, type: 'ok' },
  { text: '  ✓  AOF persistence: data/appendonly.aof', delay: 1300, type: 'ok' },
  { text: '  ✓  Ready to accept connections.', delay: 1600, type: 'ok' },
  { text: '', delay: 2200, type: 'blank' },
  { text: '> SET user makrouna', delay: 2500, type: 'input' },
  { text: '+OK', delay: 2900, type: 'resp' },
  { text: '> EXPIRE user 30', delay: 3300, type: 'input' },
  { text: ':1', delay: 3700, type: 'resp' },
  { text: '> TTL user', delay: 4100, type: 'input' },
  { text: ':29', delay: 4500, type: 'resp' },
  { text: '> PENNE dish "linguine al pesto"', delay: 4900, type: 'input' },
  { text: ':1   # set only if key doesn\'t exist', delay: 5300, type: 'resp' },
  { text: '> LASAGNA dish dish_backup', delay: 5700, type: 'input' },
  { text: '+OK', delay: 6100, type: 'resp' },
]

function TerminalWindow() {
  const [visibleCount, setVisibleCount] = useState(0)
  const timers = useRef<ReturnType<typeof setTimeout>[]>([])

  useEffect(() => {
    timers.current = TERMINAL_LINES.map((line, i) =>
      setTimeout(() => setVisibleCount((n) => Math.max(n, i + 1)), line.delay)
    )
    return () => timers.current.forEach(clearTimeout)
  }, [])

  const colorMap: Record<string, string> = {
    cmd: '#F0B243',
    ok: '#3DDC84',
    input: '#F5F1E9',
    resp: 'rgba(61,220,132,0.65)',
    comment: 'rgba(139,133,128,0.7)',
    blank: 'transparent',
  }

  return (
    <div className="terminal-glow rounded-2xl overflow-hidden border border-white/[0.09]">
      {/* Mac-style chrome */}
      <div
        className="flex items-center gap-2 px-4 py-3 border-b border-white/[0.06]"
        style={{ background: 'rgba(255,255,255,0.025)' }}
      >
        <span className="w-3 h-3 rounded-full bg-[#FF5F57]" />
        <span className="w-3 h-3 rounded-full bg-[#FFBD2E]" />
        <span className="w-3 h-3 rounded-full bg-[#28C840]" />
        <span className="ml-4 font-mono text-[11px] tracking-wide" style={{ color: 'rgba(255,255,255,0.22)' }}>
          makrounaDB — zsh — 80×24
        </span>
      </div>

      {/* Terminal body */}
      <div
        className="p-6 min-h-[380px] font-mono text-[13px] leading-[1.85]"
        style={{ background: '#0B090E' }}
      >
        {TERMINAL_LINES.map((line, i) => {
          if (i >= visibleCount) return null
          if (line.type === 'blank') return <div key={i} className="h-2" />

          // Split resp lines on '#' for inline comments
          const [main, comment] = line.text.split(/\s+#\s+/)
          return (
            <div key={i} style={{ color: colorMap[line.type] }}>
              {main}
              {comment && (
                <span style={{ color: 'rgba(139,133,128,0.55)' }}>{`  # ${comment}`}</span>
              )}
            </div>
          )
        })}

        {/* Blinking cursor */}
        {visibleCount >= TERMINAL_LINES.length && (
          <div className="flex items-center gap-0 mt-0.5">
            <span style={{ color: 'rgba(240,178,67,0.6)' }}>{'> '}</span>
            <span
              className="inline-block w-[7px] h-[15px] animate-cursor-blink"
              style={{ background: 'var(--gold)', marginLeft: '2px' }}
            />
          </div>
        )}
      </div>
    </div>
  )
}

const stagger = {
  hidden: {},
  visible: { transition: { staggerChildren: 0.11 } },
}

const item = {
  hidden: { opacity: 0, y: 22 },
  visible: {
    opacity: 1,
    y: 0,
    transition: { duration: 0.7, ease: [0.25, 0.46, 0.45, 0.94] },
  },
}

export default function Hero() {
  return (
    <section className="relative min-h-screen flex items-center dot-grid overflow-hidden">
      {/* Ambient blobs */}
      <div
        className="absolute pointer-events-none"
        style={{
          top: '20%',
          left: '10%',
          width: '500px',
          height: '500px',
          borderRadius: '50%',
          background: 'radial-gradient(circle, rgba(201,135,42,0.07) 0%, transparent 70%)',
          filter: 'blur(60px)',
        }}
      />
      <div
        className="absolute pointer-events-none"
        style={{
          bottom: '15%',
          right: '10%',
          width: '360px',
          height: '360px',
          borderRadius: '50%',
          background: 'radial-gradient(circle, rgba(61,220,132,0.04) 0%, transparent 70%)',
          filter: 'blur(60px)',
        }}
      />

      <div className="relative z-10 max-w-7xl mx-auto px-6 md:px-12 pt-28 pb-20 w-full">
        <div className="grid lg:grid-cols-[1fr_1.05fr] gap-14 lg:gap-20 items-center">
          {/* Left: Copy */}
          <motion.div variants={stagger} initial="hidden" animate="visible">
            {/* Version badge */}
            <motion.div variants={item}>
              <span
                className="inline-flex items-center gap-2 font-mono text-[11px] tracking-widest uppercase px-3 py-1.5 rounded-full border mb-7"
                style={{
                  color: 'var(--gold)',
                  borderColor: 'rgba(201,135,42,0.28)',
                  background: 'rgba(201,135,42,0.06)',
                }}
              >
                <span
                  className="w-1.5 h-1.5 rounded-full animate-pulse"
                  style={{ background: 'var(--gold)' }}
                />
                v0.1.0 — Open Source · MIT
              </span>
            </motion.div>

            {/* Hero headline */}
            <motion.h1
              variants={item}
              className="font-display leading-[0.9] tracking-tight mb-5"
              style={{
                fontSize: 'clamp(4.5rem, 11vw, 9rem)',
                color: 'var(--text)',
              }}
            >
              makrouna
              <span style={{ color: 'var(--gold)' }}>DB</span>
            </motion.h1>

            {/* Tagline */}
            <motion.p
              variants={item}
              className="font-body text-xl md:text-[1.35rem] leading-relaxed italic mb-2"
              style={{ color: 'var(--text-muted)' }}
            >
              Redis-inspired in-memory database,
              <br />
              written in modern C++.
            </motion.p>

            {/* Feature pills */}
            <motion.div variants={item} className="flex flex-wrap gap-2 mb-10 mt-5">
              {['RESP2 Protocol', 'TTL Expiration', 'AOF Persistence', 'Natural Language CLI'].map(
                (tag) => (
                  <span
                    key={tag}
                    className="font-mono text-[11px] px-2.5 py-1 rounded border"
                    style={{
                      color: 'var(--text-faint)',
                      borderColor: 'rgba(255,255,255,0.08)',
                      background: 'rgba(255,255,255,0.03)',
                    }}
                  >
                    {tag}
                  </span>
                )
              )}
            </motion.div>

            {/* CTAs */}
            <motion.div variants={item} className="flex flex-wrap gap-4">
              <a
                href="#installation"
                className="inline-flex items-center gap-2 font-mono text-sm font-semibold px-7 py-3.5 rounded-xl transition-all duration-200"
                style={{
                  background: 'var(--gold)',
                  color: '#0C0A07',
                }}
                onMouseEnter={(e) =>
                  ((e.currentTarget as HTMLElement).style.background = 'var(--gold-bright)')
                }
                onMouseLeave={(e) =>
                  ((e.currentTarget as HTMLElement).style.background = 'var(--gold)')
                }
              >
                Get Started →
              </a>
              <a
                href="https://github.com/ghassenov/makrounaDB"
                target="_blank"
                rel="noopener noreferrer"
                className="inline-flex items-center gap-2 font-mono text-sm px-7 py-3.5 rounded-xl border transition-all duration-200"
                style={{
                  borderColor: 'rgba(255,255,255,0.1)',
                  color: 'var(--text-muted)',
                }}
                onMouseEnter={(e) => {
                  const el = e.currentTarget as HTMLElement
                  el.style.borderColor = 'rgba(201,135,42,0.35)'
                  el.style.color = 'var(--gold)'
                }}
                onMouseLeave={(e) => {
                  const el = e.currentTarget as HTMLElement
                  el.style.borderColor = 'rgba(255,255,255,0.1)'
                  el.style.color = 'var(--text-muted)'
                }}
              >
                <GitHubIcon />
                View on GitHub
              </a>
            </motion.div>
          </motion.div>

          {/* Right: Terminal */}
          <motion.div
            initial={{ opacity: 0, x: 40 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.95, delay: 0.25, ease: [0.25, 0.46, 0.45, 0.94] }}
          >
            <TerminalWindow />
          </motion.div>
        </div>

        {/* Scroll nudge */}
        <motion.div
          className="absolute bottom-10 left-1/2 -translate-x-1/2 flex flex-col items-center gap-2"
          style={{ color: 'var(--text-faint)' }}
          initial={{ opacity: 0 }}
          animate={{ opacity: 1 }}
          transition={{ delay: 3.5, duration: 1.2 }}
        >
          <span className="font-mono text-[10px] tracking-[0.2em] uppercase">scroll</span>
          <div
            className="w-px h-10"
            style={{
              background: 'linear-gradient(to bottom, rgba(74,71,66,0.5), transparent)',
            }}
          />
        </motion.div>
      </div>
    </section>
  )
}

function GitHubIcon() {
  return (
    <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
      <path d="M12 0C5.374 0 0 5.373 0 12c0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23A11.509 11.509 0 0112 5.803c1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576C20.566 21.797 24 17.3 24 12c0-6.627-5.373-12-12-12z" />
    </svg>
  )
}
