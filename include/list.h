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

#ifndef LOCK_FREE_LIST_H__
#define LOCK_FREE_LIST_H__

namespace lockFree {


template <typename V>
class list {
public:
	list() : head(&dummy), tail(&dummy), dummy(nullptr) { }
	~list() = default;
	void insert(V v) {
		elem *newElem = new elem(v, &dummy);
		while (true) {
			elem *oldTail = get_ownership_and_invalidate(tail);
			if (&dummy != oldTail) {
				oldTail->next.store(newElem);
				tail.store(newElem);
				return ;
			}
			if (head.compare_exchange_strong(oldTail, newElem)) {
				tail.store(newElem);
				return ;
			}
			tail.store(oldTail);
		}
	}

	V remove() {
		while (true) {
			std::unique_ptr<elem> headElem(get_ownership_and_invalidate(head));
			if (&dummy == headElem.get()) {
				head.store(headElem.release());
				throw std::runtime_error("empty");
			}
			auto newHead = headElem->next.load();
			if (&dummy != newHead) {
				if (newHead->next.load() == &dummy && tail.load() != newHead) {
					head.store(headElem.release());
					continue;
				}
				head.store(newHead);
				return headElem->v;
			}
			elem *old_value = headElem.get();
			if (tail.compare_exchange_strong(old_value, newHead))
			{
				head.store(newHead);
				return headElem->v;
			}
			head.store(headElem.release());
		}
   	}
	bool isEmpty() const { return head.load() == &dummy; }
private:

	struct elem {
		V v;
		std::atomic<elem *> next;
		elem(elem *n) : next(n) { }
		elem(V value, elem *n) : v(value), next(n) { }
	};

	static elem *get_ownership_and_invalidate(std::atomic<elem *> &ptr) {
		auto to_take = ptr.load();
		while (nullptr == to_take || !ptr.compare_exchange_weak(to_take, nullptr)) {
			to_take = ptr.load();
		}
		return to_take;
	}

	std::atomic<elem *> head;
	std::atomic<elem *> tail;
	elem dummy;
};

}

#endif
