'use client'

import { motion } from 'framer-motion'
import { ReactNode } from 'react'

interface FadeInProps {
  children: ReactNode
  delay?: number
  direction?: 'up' | 'down' | 'left' | 'right' | 'none'
  className?: string
  once?: boolean
}

export default function FadeIn({
  children,
  delay = 0,
  direction = 'up',
  className = '',
  once = true,
}: FadeInProps) {
  const initial = {
    opacity: 0,
    y: direction === 'up' ? 28 : direction === 'down' ? -28 : 0,
    x: direction === 'left' ? 28 : direction === 'right' ? -28 : 0,
  }

  return (
    <motion.div
      className={className}
      initial={initial}
      whileInView={{ opacity: 1, y: 0, x: 0 }}
      viewport={{ once, margin: '-72px' }}
      transition={{
        duration: 0.72,
        delay,
        ease: [0.25, 0.46, 0.45, 0.94],
      }}
    >
      {children}
    </motion.div>
  )
}
