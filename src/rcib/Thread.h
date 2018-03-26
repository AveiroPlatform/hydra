/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MUL_THREAD_
#define MUL_THREAD_

namespace base {
#ifdef _WIN32
  typedef DWORD PlatformThreadId;
  typedef void* PlatformThreadHandle;  // HANDLE
#else
  typedef pthread_t PlatformThreadHandle;
  typedef pid_t PlatformThreadId;
#endif
  class Thread : public SupportsWeakPtr<Thread> {
  public:
    static const unsigned long kInvalidThreadId = 0;

    struct Options {
      Options();
      Options(MessageLoop::Type type, size_t size);

      size_t stack_size_;
      MessageLoop::Type message_loop_type_;
    };
    enum ThreadPriority {
      kThreadPriority_Normal,
      // Suitable for low-latency, glitch-resistant audio.
      kThreadPriority_RealtimeAudio,
      // Suitable for threads which generate data for the display (at ~60Hz).
      kThreadPriority_Display,
      // Suitable for threads that shouldn't disrupt high priority work.
      kThreadPriority_Background
    };
#ifdef _WIN32
    enum ComStatus {
      NONE,
      STA,
      MTA,
    };
    class ScopedCOMInitializer {
    public:
      ScopedCOMInitializer(): thread_id_(Thread::kInvalidThreadId) {
        Initialize(COINIT_APARTMENTTHREADED);
      }
      explicit ScopedCOMInitializer(ComStatus mta):thread_id_(Thread::kInvalidThreadId) {
        Initialize(COINIT_MULTITHREADED);
      }
      ~ScopedCOMInitializer();
    private:
      void Initialize(COINIT init);
      DWORD thread_id_;
    };
#endif
    struct StartupData {
      const Options &options_;
      WaitableEvent event_;

      explicit StartupData(const Options& options):
      options_(options), event_(false, false){}
    };

    struct ThreadParams {
      Thread* thread_;
    };

    explicit Thread();
    explicit Thread(const char *name);
    virtual ~Thread();

    void Init();
    bool StartWithOptions(const Options& options);
    void Stop();
    MessageLoop *message_loop() const { return message_loop_; }
    const std::string & thread_name() const { return name_; }
    bool set_thread_name(std::string name);
    PlatformThreadHandle thread_handle() { return thread_; }
    PlatformThreadId thread_id() { return thread_id_; }
    bool IsRunning() const;
    void SetPriority(ThreadPriority priority);
#ifdef _WIN32
    void init_com_with_mta(bool use_mta) {
      assert(!started_);
      com_status_ = use_mta ? MTA : STA;
    }
#endif
    virtual void ThreadMain();

    //static
    static bool CreateThreadInternal(size_t stack_size, Thread *thread, PlatformThreadHandle *out_thread_handle);

    bool SetRoler(std::string name, FurRoler &ft);
    FurRoler GetRoler(std::string name);
    void RemoveRoler(std::string name);
    void IncComputational();
    void DecComputational();
    size_t Computational();
  protected:
    virtual void CleanUp() {}
  private:
    void Run(MessageLoop *message_loop);

    MessageLoop* message_loop_;
    PlatformThreadHandle thread_;
    std::string name_;
    PlatformThreadId thread_id_;
    // Whether we successfully started the thread.
    bool started_;

    // If true, we're in the middle of stopping, and shouldn't access
    // |message_loop_|. It may non-NULL and invalid.
    bool stopping_;

    // True while inside of Run().
    bool running_;
#ifdef _WIN32
    ComStatus com_status_;
#endif
    StartupData* startup_data_;

    std::map<std::string, FurRoler> rolers_;

    typedef std::map<std::string, FurRoler>::iterator FileIter;

    size_t computational_;

    DISALLOW_COPY_AND_ASSIGN_(Thread);
  };
}

#endif
