import Nav from '@/components/Nav'
import Hero from '@/components/Hero'
import Overview from '@/components/Overview'
import Features from '@/components/Features'
import PastaCommands from '@/components/PastaCommands'
import HowItWorks from '@/components/HowItWorks'
import Architecture from '@/components/Architecture'
import Installation from '@/components/Installation'
import Footer from '@/components/Footer'

export default function Home() {
  return (
    <main className="relative">
      <Nav />
      <Hero />
      <Overview />
      <Features />
      <PastaCommands />
      <HowItWorks />
      <Architecture />
      <Installation />
      <Footer />
    </main>
  )
}
