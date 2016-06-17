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
class list2 {
public:
	list2() : head(&dummy), tail(&dummy), dummy(nullptr) {}
	~list2() = default;
	void insert(V v) {
		elem *newElem = new elem(v, &dummy);
		while (true) {
			elem *previous = get_ownership_and_invalidate(tail);
			if (&dummy == previous)
			{
				if (!head.compare_exchange_strong(previous, newElem)) {
					tail.store(previous);
				}
				else {
					tail.store(newElem);
					return ;
				}
			}
			else {
				previous->next.store(newElem);
				tail.store(newElem);
				return ;
			}
		}
	}

	V remove() {
		while (true) {
			std::unique_ptr<elem> e(get_ownership_and_invalidate(head));
			if (&dummy == e.get()) {
				head.store(e.release());
				throw std::runtime_error("empty");
			}

			auto newHead = e->next.load();
			if (newHead == &dummy) {
				elem *old_value = e.get();
				if (!tail.compare_exchange_strong(old_value, newHead))
 				{
					head.store(e.release());
					continue;
				}
			}
			head.store(newHead);
			return e->v;
		}
   	}
	bool isEmpty() { return head.load() == &dummy; }
private:

	struct elem {
		V v;
		int flag;
		std::atomic<elem *> next;
		elem(elem *n) : flag(0), next(n) {}
		elem(V value, elem *n) : v(value), flag(0), next(n) {}
	};

	/* idea:
	 * 1) push :
	 * 	- create a new elem with next == dummy
	 * 	- read tail and invalidate
	 * 	- if (tail == dummy)
	 * 	     replace head by elem
	 * 	- tail -> next = elem
	 * 	- replace tail by elem
	 * 	2) pop:
	 * 	 - get and invalidate the head
	 * 	 - if dummy element
	 * 	        reset the head return 'empty'
	 * 	 - if head->next is dummy
	 * 	        read tail and invalidate
	 * 	        if tail == elem
	 * 	           tail = head = dummy (head first so that pop is empty and push cannot be validate while the head is not set)
 	 * 	        else
	 * 	           reset tail and head and retry (head first)
	 * 	   else
	 * 	        replace head by head->next
	 * 	   return head
	 */

	elem *get_ownership_and_invalidate(std::atomic<elem *> &ptr) {
		auto to_take = ptr.load();
		while (nullptr == to_take || !ptr.compare_exchange_weak(to_take, nullptr)) { to_take = ptr.load(); }
		return to_take;
	}

	std::atomic<elem *> head;
	std::atomic<elem *> tail;
	elem dummy;
};

}

#endif
