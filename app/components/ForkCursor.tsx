'use client'

import { useEffect, useState } from 'react'

export default function ForkCursor() {
  const [pos, setPos] = useState({ x: -300, y: -300 })
  const [isDown, setIsDown] = useState(false)
  const [isVisible, setIsVisible] = useState(false)
  const [isHover, setIsHover] = useState(false)

  useEffect(() => {
    const onMove = (e: MouseEvent) => {
      setPos({ x: e.clientX, y: e.clientY })
      if (!isVisible) setIsVisible(true)

      // Detect hovering over interactive elements
      const target = e.target as HTMLElement
      const interactive = target.closest('a, button, [role="button"], input, textarea, select, label')
      setIsHover(!!interactive)
    }

    const onDown = () => setIsDown(true)
    const onUp = () => setIsDown(false)
    const onLeave = () => setIsVisible(false)
    const onEnter = () => setIsVisible(true)

    window.addEventListener('mousemove', onMove)
    window.addEventListener('mousedown', onDown)
    window.addEventListener('mouseup', onUp)
    document.documentElement.addEventListener('mouseleave', onLeave)
    document.documentElement.addEventListener('mouseenter', onEnter)

    return () => {
      window.removeEventListener('mousemove', onMove)
      window.removeEventListener('mousedown', onDown)
      window.removeEventListener('mouseup', onUp)
      document.documentElement.removeEventListener('mouseleave', onLeave)
      document.documentElement.removeEventListener('mouseenter', onEnter)
    }
  }, [isVisible])

  const rotation = isDown ? 30 : isHover ? -5 : -22
  const scale = isDown ? 0.82 : isHover ? 1.15 : 1
  const glowSize = isDown ? 14 : isHover ? 9 : 5
  const glowOpacity = isDown ? 0.75 : isHover ? 0.55 : 0.35
  const color = isDown ? '#F0B243' : isHover ? '#E8A030' : '#C9872A'

  return (
    <>
      {/* Hide native cursor everywhere */}
      <style>{`html, html * { cursor: none !important; }`}</style>

      {/* Fork cursor — positioned at exact mouse coords */}
      <div
        aria-hidden="true"
        style={{
          position: 'fixed',
          top: 0,
          left: 0,
          pointerEvents: 'none',
          zIndex: 99999,
          transform: `translate(${pos.x}px, ${pos.y}px)`,
          willChange: 'transform',
        }}
      >
        {/* Inner wrapper: apply rotation + scale from the handle center */}
        <div
          style={{
            /* Center fork horizontally; tip of tines at y=0 (cursor hotpoint) */
            transform: `translateX(-9px) rotate(${rotation}deg) scale(${scale})`,
            transformOrigin: '9px 28px',
            transition: 'transform 0.18s cubic-bezier(0.34, 1.56, 0.64, 1)',
            opacity: isVisible ? 1 : 0,
            filter: `drop-shadow(0 0 ${glowSize}px rgba(201,135,42,${glowOpacity}))`,
            color,
          }}
        >
          <ForkSVG />
        </div>
      </div>
    </>
  )
}

function ForkSVG() {
  return (
    <svg
      width="18"
      height="38"
      viewBox="0 0 18 38"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      {/* ── 4 tines ── */}
      <rect x="0.5"  y="0" width="3" height="14" rx="1.5" fill="currentColor" />
      <rect x="5"    y="0" width="3" height="14" rx="1.5" fill="currentColor" />
      <rect x="9.5"  y="0" width="3" height="14" rx="1.5" fill="currentColor" />
      <rect x="14"   y="0" width="3" height="14" rx="1.5" fill="currentColor" />

      {/* ── Bridge connecting tines ── */}
      <rect x="0.5" y="12" width="16.5" height="2.5" rx="1.25" fill="currentColor" />

      {/* ── Neck (tapers into handle) ── */}
      <path
        d="M8 14.5 L10 14.5 L10.5 19.5 L7.5 19.5 Z"
        fill="currentColor"
      />

      {/* ── Handle ── */}
      <rect x="6.25" y="19" width="5.5" height="18.5" rx="2.75" fill="currentColor" />

      {/* ── Subtle highlight on handle ── */}
      <rect
        x="7.5"
        y="21"
        width="1.5"
        height="12"
        rx="0.75"
        fill="white"
        opacity="0.12"
      />
    </svg>
  )
}
