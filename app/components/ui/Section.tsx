import { ReactNode } from 'react'

interface SectionProps {
  id?: string
  className?: string
  children: ReactNode
}

export default function Section({ id, className = '', children }: SectionProps) {
  return (
    <section
      id={id}
      className={`relative px-6 md:px-12 lg:px-24 py-24 md:py-32 section-divider ${className}`}
    >
      {children}
    </section>
  )
}
