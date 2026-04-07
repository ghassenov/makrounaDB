import type { Config } from 'tailwindcss'

const config: Config = {
  content: [
    './pages/**/*.{js,ts,jsx,tsx,mdx}',
    './components/**/*.{js,ts,jsx,tsx,mdx}',
    './app/**/*.{js,ts,jsx,tsx,mdx}',
  ],
  theme: {
    extend: {
      fontFamily: {
        display: ['var(--font-bebas)', 'Impact', 'cursive'],
        heading: ['var(--font-syne)', 'sans-serif'],
        body: ['var(--font-crimson)', 'Georgia', 'serif'],
        mono: ['var(--font-jetbrains)', 'Courier New', 'monospace'],
      },
      colors: {
        gold: '#C9872A',
        'gold-bright': '#F0B243',
        'gold-dim': 'rgba(201,135,42,0.12)',
        ink: '#F5F1E9',
        'ink-muted': '#8B8580',
        surface: '#111013',
      },
      animation: {
        'cursor-blink': 'cursor-blink 1s step-end infinite',
        'float': 'float 6s ease-in-out infinite',
      },
      keyframes: {
        'cursor-blink': {
          '0%, 100%': { opacity: '1' },
          '50%': { opacity: '0' },
        },
        'float': {
          '0%, 100%': { transform: 'translateY(0px)' },
          '50%': { transform: 'translateY(-8px)' },
        },
      },
    },
  },
  plugins: [],
}

export default config
