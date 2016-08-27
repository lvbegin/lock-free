/* Copyright 2016 Laurent Van Begin
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
  */

#ifndef LOCK_FREE_STACK_H__
#define LOCK_FREE_STACK_H__

#include <atomic>
#include <memory>
#include <exception>

namespace lockFree {

template <typename V>
class stack {
public:
	stack() : tail(nullptr), head(nullptr) {
		head.store(&tail);
	}
	~stack()  = default;
	struct stackElem {
		V v;
		stackElem *next;
		stackElem(V value, stackElem *nextElem) : v(value), next(nextElem) { }
		stackElem(stackElem *nextElem) : next(nextElem) { }

	};
	void push(V v) {
		stackElem *elem = new stackElem(v, head.load());

		while (nullptr == elem->next || !head.compare_exchange_weak(elem->next, elem))
			elem->next = head.load();

	}
	V pop() {
		stackElem *elem = head.load();
		while (nullptr == elem || !head.compare_exchange_weak(elem, nullptr)) {
			elem = head.load();
		}
		if (elem == &tail) {
			head.store(elem);
			throw std::runtime_error("empty");
		}

		head.store(elem->next);
		std::unique_ptr<stackElem> toDelete(elem);
		return toDelete->v;
	}
	bool isEmpty() const { return head.load() == &tail; }
private:
	stackElem tail;
	std::atomic<stackElem *> head;
};

}
#endif
