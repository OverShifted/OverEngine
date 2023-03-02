import "time" for Time

class WaitSeconds {
	construct new(seconds) {
		_seconds = seconds
	}

	seconds { _seconds }
}

class Schedule {
	construct new(resumeTime, fiber) {
		_resumeTime = resumeTime
		_fiber = fiber
	}

	resumeTime { _resumeTime }
	fiber { _fiber }
}

class Scheduler {
	static init() {
		__scheduled = []
	} 

	static add(fn) {
		addFiber(Fiber.new { fn.call() })
	}

	static addFiber(fib) {
		var result = fib.call()

		if (!fib.isDone) {
			if (!(result is WaitSeconds)) {
				Fiber.abort("result is not of type 'WaitSeconds'.")
			}

			var resumeTime = Time.time + result.seconds
			__scheduled.add(Schedule.new(resumeTime, fib))
		}
	}

	static poll() {
		var i = __scheduled.count - 1
		while (i >= 0) {
			var schedule = __scheduled[i]
			if (schedule.resumeTime <= Time.time) {
				addFiber(__scheduled.removeAt(i).fiber)
			}
			i = i - 1
		}
	}

	static count { __scheduled.count }
}

Scheduler.init()
