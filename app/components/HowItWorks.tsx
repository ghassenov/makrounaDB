import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'
import CodeBlock from '@/components/ui/CodeBlock'

const steps = [
  {
    number: '01',
    title: 'Build the Server',
    description:
      'Compile makrounaDB from source using CMake and Ninja. A single self-contained binary is produced — no runtime dependencies.',
    code: `# Install prerequisites
sudo apt install -y g++ cmake ninja-build

# Configure and build
cmake -S . -B build -G Ninja
cmake --build build
# → ./build/makrounaDB`,
  },
  {
    number: '02',
    title: 'Start It Up',
    description:
      'Launch the server on any port. Pass an AOF file path to enable persistence. The server is ready to accept RESP2 connections immediately.',
    code: `# Minimal start
./build/makrounaDB

# With explicit options
./build/makrounaDB --port 6379 --aof data/appendonly.aof

# Show all options
./build/makrounaDB --help`,
  },
  {
    number: '03',
    title: 'Connect & Query',
    description:
      'Use any Redis client, send raw RESP2 frames via netcat, run the included Python example, or launch the natural language shell.',
    code: `# Natural language shell
./build/makrounaDB --nl-shell --host 127.0.0.1 --port 6379

# Type commands like:
#   set user to ghassen
#   what is user?
#   expire user in 30 seconds
#   does user exist?

# Or send raw RESP2 via nc
printf '*3\\r\\n$3\\r\\nSET\\r\\n$4\\r\\nname\\r\\n$10\\r\\nmakrounaDB\\r\\n' \\
  | nc -N 127.0.0.1 6379`,
  },
  {
    number: '04',
    title: 'Benchmark & Ship',
    description:
      'Run the built-in benchmark suite to measure throughput, or export .deb / .rpm packages for deployment.',
    code: `# Run test suite
ctest --test-dir build --output-on-failure

# Benchmark (generates benchmarks/report.md)
python3 benchmarks/simple_benchmark.py \\
  --host 127.0.0.1 --port 6379 --requests 5000

# Build release packages
bash scripts/release/build_release.sh --format all`,
  },
]

export default function HowItWorks() {
  return (
    <Section id="how-it-works">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // how it works
          </p>
        </FadeIn>

        <FadeIn delay={0.08}>
          <h2
            className="font-heading text-4xl md:text-[2.8rem] font-bold leading-tight mb-16"
            style={{ color: 'var(--text)' }}
          >
            Up and running{' '}
            <span style={{ color: 'var(--gold)' }}>in minutes.</span>
          </h2>
        </FadeIn>

        <div className="space-y-20">
          {steps.map((step, i) => (
            <FadeIn key={step.number} delay={0.08}>
              <div className="grid lg:grid-cols-2 gap-10 lg:gap-16 items-start">
                {/* Text */}
                <div className={i % 2 === 1 ? 'lg:order-2' : ''}>
                  <div className="flex items-start gap-5">
                    <span
                      className="font-display leading-none shrink-0"
                      style={{
                        fontSize: '4.5rem',
                        color: 'rgba(201,135,42,0.15)',
                        lineHeight: 1,
                      }}
                    >
                      {step.number}
                    </span>
                    <div>
                      <h3
                        className="font-heading text-2xl font-bold mb-3"
                        style={{ color: 'var(--text)' }}
                      >
                        {step.title}
                      </h3>
                      <p
                        className="font-body text-[1.05rem] leading-relaxed"
                        style={{ color: 'var(--text-muted)' }}
                      >
                        {step.description}
                      </p>
                    </div>
                  </div>
                </div>

                {/* Code */}
                <div className={i % 2 === 1 ? 'lg:order-1' : ''}>
                  <CodeBlock code={step.code} lang="bash" />
                </div>
              </div>

              {/* Connector line between steps */}
              {i < steps.length - 1 && (
                <div
                  className="mt-20 mx-auto w-px h-12"
                  style={{
                    background: 'linear-gradient(to bottom, rgba(201,135,42,0.2), transparent)',
                  }}
                />
              )}
            </FadeIn>
          ))}
        </div>
      </div>
    </Section>
  )
}
