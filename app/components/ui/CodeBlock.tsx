'use client'

import { useState } from 'react'

interface CodeBlockProps {
  code: string
  lang?: string
  showCopy?: boolean
}

export default function CodeBlock({ code, lang = 'bash', showCopy = true }: CodeBlockProps) {
  const [copied, setCopied] = useState(false)

  const handleCopy = async () => {
    await navigator.clipboard.writeText(code)
    setCopied(true)
    setTimeout(() => setCopied(false), 2000)
  }

  return (
    <div className="relative rounded-xl overflow-hidden border border-white/[0.07] bg-[#0A080D]">
      {/* Window chrome */}
      <div className="flex items-center justify-between px-4 py-2.5 border-b border-white/[0.05] bg-white/[0.02]">
        <div className="flex items-center gap-1.5">
          <div className="w-2.5 h-2.5 rounded-full bg-[#FF5F57]/70" />
          <div className="w-2.5 h-2.5 rounded-full bg-[#FFBD2E]/70" />
          <div className="w-2.5 h-2.5 rounded-full bg-[#28C840]/70" />
          <span className="ml-3 text-[11px] font-mono text-white/20 tracking-wide">{lang}</span>
        </div>
        {showCopy && (
          <button
            onClick={handleCopy}
            className="text-[11px] font-mono text-white/25 hover:text-[#F0B243] transition-colors duration-200 select-none"
            aria-label="Copy code"
          >
            {copied ? '✓ copied' : 'copy'}
          </button>
        )}
      </div>
      {/* Code content */}
      <pre className="p-5 overflow-x-auto">
        <code
          className="font-mono text-[13px] leading-relaxed whitespace-pre"
          style={{ color: 'var(--green)' }}
        >
          {code}
        </code>
      </pre>
    </div>
  )
}
