import Section from '@/components/ui/Section'
import FadeIn from '@/components/ui/FadeIn'
import CodeBlock from '@/components/ui/CodeBlock'

const BUILD_FROM_SOURCE = `# 1. Prerequisites
sudo apt update
sudo apt install -y g++ cmake ninja-build python3 netcat-openbsd

# 2. Clone
git clone https://github.com/ghassenov/makrounaDB.git
cd makrounaDB

# 3. Build
cmake -S . -B build -G Ninja
cmake --build build

# 4. Run
./build/makrounaDB --port 6379 --aof data/appendonly.aof`

const DOCKER_CODE = `# Pull the image
docker pull makrounadb/makrounadb:latest

# Run with default settings (port 6379)
docker run -p 6379:6379 makrounadb/makrounadb:latest

# Run with persistent AOF volume
docker run -p 6379:6379 \\
  -v $(pwd)/data:/data \\
  makrounadb/makrounadb:latest \\
  --aof /data/appendonly.aof`

const RELEASE_CODE = `# Build .deb package
bash scripts/release/build_release.sh --format deb

# Build .rpm package
bash scripts/release/build_release.sh --format rpm

# Build all formats + publish GitHub Release
bash scripts/release/build_release.sh \\
  --format all --github-release --tag v0.1.0`

export default function Installation() {
  return (
    <Section id="installation">
      <div className="max-w-7xl mx-auto">
        <FadeIn>
          <p
            className="font-mono text-[11px] tracking-[0.25em] uppercase mb-5"
            style={{ color: 'var(--gold)' }}
          >
            // installation
          </p>
        </FadeIn>

        <FadeIn delay={0.08}>
          <h2
            className="font-heading text-4xl md:text-[2.8rem] font-bold mb-4"
            style={{ color: 'var(--text)' }}
          >
            Start in seconds.
          </h2>
          <p className="font-body text-[1.1rem] mb-16 max-w-2xl" style={{ color: 'var(--text-muted)' }}>
            Build from source with CMake, run as a Docker container, or grab a native{' '}
            <code className="font-mono text-sm" style={{ color: 'var(--gold)' }}>.deb</code> /{' '}
            <code className="font-mono text-sm" style={{ color: 'var(--gold)' }}>.rpm</code> package.
          </p>
        </FadeIn>

        {/* Primary: Build from source + side cards */}
        <div className="grid lg:grid-cols-[1fr_320px] gap-8 mb-10">
          <FadeIn delay={0.1}>
            <div className="flex items-center gap-3 mb-5">
              <StepBadge n={1} />
              <h3 className="font-heading text-xl font-semibold" style={{ color: 'var(--text)' }}>
                Build from Source
              </h3>
            </div>
            <CodeBlock code={BUILD_FROM_SOURCE} lang="bash" />
          </FadeIn>

          <div className="space-y-4">
            <FadeIn delay={0.16} direction="left">
              <div
                className="side-card p-5 rounded-xl"
                style={{
                  border: '1px solid rgba(255,255,255,0.07)',
                  background: 'rgba(255,255,255,0.02)',
                }}
              >
                <h4 className="font-heading text-sm font-semibold mb-2 flex items-center gap-2" style={{ color: 'var(--text)' }}>
                  <span role="img">🐳</span> Docker
                </h4>
                <p className="font-body text-sm" style={{ color: 'var(--text-muted)' }}>
                  Container image in the works. Run anywhere with zero build step.
                </p>
              </div>
            </FadeIn>

            <FadeIn delay={0.24} direction="left">
              <div
                className="side-card p-5 rounded-xl"
                style={{
                  border: '1px solid rgba(255,255,255,0.07)',
                  background: 'rgba(255,255,255,0.02)',
                }}
              >
                <h4 className="font-heading text-sm font-semibold mb-2 flex items-center gap-2" style={{ color: 'var(--text)' }}>
                  <span role="img">📦</span> Release Packages
                </h4>
                <p className="font-body text-sm mb-2" style={{ color: 'var(--text-muted)' }}>
                  Export native packages using the included release scripts.
                </p>
                <code className="font-mono text-[12px]" style={{ color: 'rgba(61,220,132,0.6)' }}>
                  scripts/release/build_release.sh
                </code>
              </div>
            </FadeIn>

            <FadeIn delay={0.32} direction="left">
              <div
                className="side-card p-5 rounded-xl"
                style={{
                  border: '1px solid rgba(255,255,255,0.07)',
                  background: 'rgba(255,255,255,0.02)',
                }}
              >
                <h4 className="font-heading text-sm font-semibold mb-2 flex items-center gap-2" style={{ color: 'var(--text)' }}>
                  <span role="img">🧪</span> Run Tests
                </h4>
                <code className="font-mono text-[12px]" style={{ color: 'rgba(61,220,132,0.6)' }}>
                  ctest --test-dir build --output-on-failure
                </code>
              </div>
            </FadeIn>
          </div>
        </div>

        {/* Docker + Release code */}
        <div className="grid lg:grid-cols-2 gap-8">
          <FadeIn delay={0.18}>
            <div className="flex items-center gap-3 mb-5">
              <StepBadge n={2} />
              <h3 className="font-heading text-xl font-semibold" style={{ color: 'var(--text)' }}>
                Docker{' '}
              </h3>
            </div>
            <CodeBlock code={DOCKER_CODE} lang="bash" />
          </FadeIn>

          <FadeIn delay={0.24}>
            <div className="flex items-center gap-3 mb-5">
              <StepBadge n={3} />
              <h3 className="font-heading text-xl font-semibold" style={{ color: 'var(--text)' }}>
                Release Artifacts
              </h3>
            </div>
            <CodeBlock code={RELEASE_CODE} lang="bash" />
          </FadeIn>
        </div>

        {/* CTA Banner */}
        <FadeIn delay={0.3}>
          <div
            className="mt-14 p-8 rounded-2xl flex flex-col md:flex-row items-start md:items-center justify-between gap-6"
            style={{
              border: '1px solid rgba(201,135,42,0.2)',
              background: 'linear-gradient(135deg, rgba(201,135,42,0.06) 0%, rgba(255,255,255,0.02) 100%)',
            }}
          >
            <div>
              <h3 className="font-heading text-2xl font-bold mb-1" style={{ color: 'var(--text)' }}>
                Ready to try makrounaDB?
              </h3>
              <p className="font-body text-base" style={{ color: 'var(--text-muted)' }}>
                Clone, build, and have a server running in under 60 seconds.
              </p>
            </div>
            <div className="flex gap-4 shrink-0">
              <a
                href="https://github.com/ghassenov/makrounaDB"
                target="_blank"
                rel="noopener noreferrer"
                className="btn-gold inline-flex items-center gap-2 font-mono text-sm font-semibold px-6 py-3 rounded-xl"
              >
                Clone on GitHub →
              </a>
              <a
                href="#commands"
                className="btn-outline inline-flex items-center gap-2 font-mono text-sm px-6 py-3 rounded-xl"
              >
                View Commands
              </a>
            </div>
          </div>
        </FadeIn>
      </div>
    </Section>
  )
}

function StepBadge({ n }: { n: number }) {
  return (
    <div
      className="w-8 h-8 rounded-lg flex items-center justify-center shrink-0 font-mono text-xs font-semibold"
      style={{
        border: '1px solid rgba(201,135,42,0.3)',
        background: 'rgba(201,135,42,0.08)',
        color: 'var(--gold)',
      }}
    >
      {n}
    </div>
  )
}
