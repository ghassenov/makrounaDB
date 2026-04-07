import type { Metadata } from 'next'
import { Bebas_Neue, Syne, Crimson_Pro, JetBrains_Mono } from 'next/font/google'
import ForkCursor from '@/components/ForkCursor'
import './globals.css'

const bebasNeue = Bebas_Neue({
  subsets: ['latin'],
  variable: '--font-bebas',
  weight: '400',
  display: 'swap',
})

const syne = Syne({
  subsets: ['latin'],
  variable: '--font-syne',
  weight: ['400', '500', '600', '700', '800'],
  display: 'swap',
})

const crimsonPro = Crimson_Pro({
  subsets: ['latin'],
  variable: '--font-crimson',
  weight: ['300', '400', '500', '600'],
  style: ['normal', 'italic'],
  display: 'swap',
})

const jetbrainsMono = JetBrains_Mono({
  subsets: ['latin'],
  variable: '--font-jetbrains',
  weight: ['300', '400', '500', '600'],
  display: 'swap',
})

export const metadata: Metadata = {
  title: 'makrounaDB — Redis-Inspired In-Memory Database in C++',
  description:
    'A fast, Redis-compatible in-memory key-value store written in modern C++. Supports RESP2 protocol, TTL-based expiration, AOF persistence, and a natural language CLI.',
  openGraph: {
    title: 'makrounaDB',
    description: 'Redis-inspired in-memory database written in modern C++.',
    type: 'website',
  },
}

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html
      lang="en"
      className={`${bebasNeue.variable} ${syne.variable} ${crimsonPro.variable} ${jetbrainsMono.variable}`}
    >
      <body>
        <ForkCursor />
        {children}
      </body>
    </html>
  )
}
